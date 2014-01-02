(proclaim '(optimize (debug 3)))

; (defconstant +default-tests-path+ "./tests.sexpr")
(defparameter +default-tests-path+       "./tests.sexpr")
(defparameter +default-ip+               "127.0.0.1")
(defparameter +default-username+         "root")
(defparameter +default-password+         "letmein")
(defparameter +default-database+         "cryptdbtest")
(defparameter +default-control-database+ "cryptdbtest_control")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;     database helpers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun db-connect (db port &optional (ip       +default-ip+)
                                     (username +default-username+)
                                     (password +default-password+))
  (clsql:connect
    `(,ip ,db ,username ,password ,port)
    :database-type :mysql
    :if-exists     :new
    :make-default  nil))

(defun issue-query (query database &optional just-return-status)
  (handler-case
        (let ((results (clsql:query query :database database)))
          (if just-return-status 'query-success results))
    (clsql:sql-database-error (e)
      (declare (ignore e))
      'query-error)))

(defun must-succeed-query (query database)
  (assert (eq 'query-success (issue-query query database t))))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;        misc helpers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; by _death from #lisp
(defun traverse (function structure path)
  (if (atom structure)
      (funcall function (reverse (cons structure path)))
      (dolist (x (cdr structure))
        (traverse function x (cons (car structure) path)))))

;; by _death from #lisp
(defmacro do-structure (((&rest vars) structure-form) &body forms)
  (let ((list (gensym)))
    `(traverse (lambda (,list)
                 (destructuring-bind ,vars ,list
                   (declare (ignorable ,@vars))
                   ,@forms))
               ,structure-form
               '())))

(defun read-file (path)
  (with-open-file (stream path :direction :input)
    (let ((seq (make-string (file-length stream))))
      (read-sequence seq stream)
      seq)))

(defun str-assoc (key alist)
  (assoc key alist :test #'string-equal))

;;; returns (did-the-lookup-complete? completed-portion-of-lookup unmatched-keys)
(defun many-str-assoc (keys nested-alists)
  (let ((alist nested-alists)
        (keys-len (length keys)))
    (do ((i 0 (1+ i)))
        ((= keys-len i) (values t alist '()))
      (let* ((k (elt keys i))
             (found-alist (str-assoc k (if (zerop i) alist (cdr alist)))))
        (when (null found-alist)
          (return (values nil alist (subseq keys i))))
        (setf alist found-alist)))))

;;; apply fn to each node
(defun map-tree (fn tree)
  (cond ((null tree) '())
        ((atom (car tree))
         (cons (funcall fn (car tree)) (map-tree fn (cdr tree))))
        (t (cons (map-tree fn (car tree)) (map-tree fn (cdr tree))))))

(defun list-depth (list)
  (assert (listp list))
  (cond ((null list) 1)
        (t (apply
             #'max
             (mapcar
               #'(lambda (e)
                   (cond ((atom e) 1)
                         (t (1+ (list-depth e)))))
               list)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;   handle test file input
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun load-tests (&optional (path +default-tests-path+))
  (read-from-string (read-file path)))

;; FIXME: implement
(defun proper-onion-update? (onion-check)
  (assert (listp onion-check))
  t)

(defun build-update-1 (database table field onion seclevel)
  `(:update (,database (,table (,field (,onion ,seclevel))))))

;;; take possibly shorthand onion-checks and produce full length checks
;;; > return nil if `dirty-onion-checks` are invalid
(defun low-level-fixup-onion-checks (dirty-onion-checks default-database)
  (let ((default-db-name (if (eq t default-database) +default-database+ default-database)))
    (cond ((atom dirty-onion-checks)
           (when (eq dirty-onion-checks :check)
             `(,dirty-onion-checks)))
          ((eq :check (car dirty-onion-checks))
           (when (= 1 (length dirty-onion-checks))
             dirty-onion-checks))
          ((eq :all-max (car dirty-onion-checks))
           (when (every #'atom dirty-onion-checks)
                   ;; (:check <table>)
             (cond ((= 2 (length dirty-onion-checks))
                    `(:all-max ,default-db-name ,(cadr dirty-onion-checks)))
                   ;; (:check <database> <table>)
                   ((= 3 (length dirty-onion-checks))
                    dirty-onion-checks))))
          ((eq :update (car dirty-onion-checks))
           (cond ((every #'atom dirty-onion-checks)
                   ;; (:update <table> <field> <onion> <seclevel>)
                  (cond ((= 4 (length (cdr dirty-onion-checks)))
                         (apply #'build-update-1 default-db-name (cdr dirty-onion-checks)))
                        ;; (:update <database> <table> <field> <onion> <seclevel>)
                        ((= 5 (length (cdr dirty-onion-checks)))
                         (apply #'build-update-1 (cdr dirty-onion-checks)))))
                   ;; (:update (<database> (<table> ...)) (<database> ...))
                   ((every #'proper-onion-update? (cdr dirty-onion-checks))
                    (cond ((= 4 (list-depth (cdr dirty-onion-checks)))
                           `(:update (,default-db-name ,@(cdr dirty-onion-checks))))
                          ((= 5 (list-depth (cdr dirty-onion-checks)))
                           dirty-onion-checks))))))))

(defun fixup-onion-checks (dirty-onion-checks default-database)
  (let ((fixed (low-level-fixup-onion-checks dirty-onion-checks default-database)))
    ;; convert symbols to strings except for the initial directive
    (when fixed
      (cons (car fixed) (map-tree #'string (cdr fixed))))))

(defun fixup-execution-target (dirty-execution-target)
  (cond ((null dirty-execution-target) :both)
        ((member dirty-execution-target '(:cryptdb :control :both))
         dirty-execution-target)
        (t nil)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;  manage local onion state
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; ((database-name
;;;    (table-name (field-name (onion-name security-level) (onion-name security-level))
;;;                (field-name (...))))
;;;  (database-name
;;;    (table-name (...))
;;;  ...))
(defstruct onion-state
  databases)

(defmethod low-level-lookup-seclevel ((onions onion-state) database table field onion)
  (let ((table-data (cdr (str-assoc database (onion-state-databases onions)))))
    (assert table-data)
    (let ((field-data (cdr (str-assoc table table-data))))
      (assert field-data)
      (let ((onion-data (cdr (str-assoc field field-data))))
        (assert onion-data)
        (let ((seclevel (cdr (str-assoc onion onion-data))))
          (assert seclevel)
          seclevel)))))

(defmethod lookup-seclevel ((onions onion-state) database table field onion)
  (car (low-level-lookup-seclevel onions database table field onion)))

(defun (setf lookup-seclevel) (seclevel onions database table field onion)
  (setf (car (low-level-lookup-seclevel onions database table field onion))
        seclevel))

(defun nest-unmatched-keys (unmatched-keys seclevel)
  (assert (not (null unmatched-keys)))
  (cond ((null (cdr unmatched-keys)) `(,(car unmatched-keys) ,seclevel))
        (t `(,(car unmatched-keys) ,(nest-unmatched-keys (cdr unmatched-keys) seclevel)))))

(defmethod add-onion! ((onions onion-state) database table field onion seclevel)
  ;; don't try to look anything up if we don't have any onion data
  (when (null (onion-state-databases onions))
    (setf (onion-state-databases onions)
          `((,database (,table (,field (,onion ,seclevel))))))
    (return-from add-onion!))
  (multiple-value-bind (success lookup unmatched-keys)
        (many-str-assoc `(,database ,table ,field ,onion)
                        (onion-state-databases onions))
    ;; a total match should only occur when we max back to back tables
    (when success
      (assert (zerop (length unmatched-keys)))
      (assert (string-equal seclevel (cadr lookup)))
      (return-from add-onion! t))
    (assert (not (null lookup)))
    (setf (cdr lookup)
          (append (cdr lookup)
                  `(,(nest-unmatched-keys unmatched-keys seclevel))))))

(defun max-level? (onion seclevel)
  (cond ((member onion '("oDET" "oOPE" "oPLAIN") :test #'string-equal)
         (string-equal "RND" seclevel))
        ((string-equal "oAGG" onion) (string-equal "HOM" seclevel))))

;;; change our local copy of the onion state
(defmethod update-onion-state! ((onions onion-state) onion-check)
  (assert (eq :update (car onion-check)))
  (dolist (checks (cdr onion-check))
    (do-structure ((database table field onion seclevel) checks)
      (setf (lookup-seclevel onions database table field onion) seclevel))))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;    onion check handling
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defstruct connection-state
  cryptdb
  plain)

(defun get-cryptdb-show (connections)
  (issue-query "SET @cryptdb='show'" (connection-state-cryptdb connections)))

(defgeneric handle-check (connections onions type onion-check)
  (:method (connections (onions onion-state) (type (eql :all-max)) onion-check)
    (let ((max-database (cadr onion-check))
          (max-table    (caddr onion-check))
          (results      (get-cryptdb-show connections))
          (output       t))
      (dolist (row results output)
        (destructuring-bind (database table field onion seclevel id) row
          (declare (ignore id))
          (unless (or (not (string-equal max-database database))
                      (not (string-equal max-table table))
                      (max-level? onion seclevel))
            (setf output nil))
          ;; update our local copy of onion state
          ;; > we cannot use (setf lookup-seclevel) because the local onion state
          ;;   may be nil
          (add-onion! onions database table field onion seclevel)))))
  (:method (connections (onions onion-state) (type (eql :update)) onion-check)
    (update-onion-state! onions onion-check)
    (handle-check connections onions :check onion-check))
  (:method (connections (onions onion-state) (type (eql :check)) onion-check)
    (let ((results (get-cryptdb-show connections))
          (output  t))
      (dolist (row results output)
        (destructuring-bind (database table field onion seclevel id) row
          (declare (ignore id))
          (unless (string-equal seclevel (lookup-seclevel onions database table field onion))
            (setf output nil))
          (setf (lookup-seclevel onions database table field onion) seclevel))))))

;;; take the per query onion checks and use them to update our onion state;
;;; then compare this new onion state to the state reported by cryptdb for
;;; each testing onion
(defmethod handle-onion-checks (connections (onions onion-state) onion-check)
  (if (null onion-check)
      t
      (handle-check connections onions (car onion-check) onion-check)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;         run tests
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defstruct group-score
  (wins 0)
  (fails 0))

(defmethod update-score ((value (eql 'query-error)))
  (declare (special *score*))
  (incf (group-score-fails *score*)))

(defmethod update-score ((value (eql 'query-success)))
  (declare (special *score*))
  (incf (group-score-wins *score*)))

(defmethod update-score ((value (eql t)))
  (declare (special *score*))
  (incf (group-score-wins *score*)))

(defmethod update-score ((value (eql nil)))
  (declare (special *score*))
  (incf (group-score-fails *score*)))

(defun fast-compare (results-a results-b)
  (equal results-a results-b))

(defun slow-compare (results-a results-b)
  (declare (ignore results-a results-b))
  nil)
  ; (error "implement slow-compare"))

(defgeneric compare-results (results-a results-b)
  (:method ((results-a list) (results-b list))
    ;; cryptdb returns all results as strings while the normal database
    ;; uses numbers and such
    (let ((results-a (all-strings results-a))
          (results-b (all-strings results-b)))
      (or (fast-compare results-a results-b) (slow-compare results-a results-b))))
  (:method ((results-a (eql 'query-error)) (results-b (eql 'query-error)))
    t)
  (:method ((results-a (eql 'query-success)) (results-b (eql 'query-success)))
    t)
  (:method (results-a results-b)
    nil))

(defun all-strings (results)
  (mapcar #'(lambda (row)
              (mapcar #'(lambda (e)
                          (if (null e)
                              "NULL"
                             (format nil "~A" e)))
                         row))
          results))

(defun valid-group-default? (group-default)
  (or (equal t group-default) (equal nil group-default) (stringp group-default)))

(defun run-test-group (connections test-group)
  (let* ((score (make-group-score))
         (*score* score)
         (onions (make-onion-state))
         (group-name (car test-group))
         (group-default (cadr test-group))
         (test-list (cddr test-group)))
    (declare (special *score*) (ignore group-name))
    (assert (valid-group-default? group-default))
    (dolist (test-case test-list score)
      (let* ((query (car test-case))
             (onion-checks (fixup-onion-checks (cadr test-case) group-default))
             (execution-target (fixup-execution-target (caddr test-case)))
             (cryptdb (connection-state-cryptdb connections))
             (control (connection-state-plain connections)))
        (assert (eq (null (cadr test-case)) (null onion-checks)))
        (case execution-target
          (:cryptdb (update-score (issue-query query cryptdb t)))
          (:control (update-score (issue-query query control t)))
          (:both
            (let ((cryptdb-results
                    (issue-query query (connection-state-cryptdb connections)))
                  (plain-results
                    (issue-query query (connection-state-plain connections))))
              (update-score
                (and (compare-results cryptdb-results plain-results)
                     (handle-onion-checks connections onions onion-checks)))))
          (otherwise (error "unknown execution-target!")))))))

(defun run-tests (all-test-groups)
  (let* ((connections
           (make-connection-state :cryptdb (db-connect nil 3307)
                                  :plain   (db-connect nil 3306)))
         (cryptdb (connection-state-cryptdb connections))
         (plain (connection-state-plain connections))
         (results '()))
    ;; remove artefacts
    (issue-query (format nil "DROP DATABASE ~A" +default-database+) cryptdb t)
    (issue-query (format nil "DROP DATABASE ~A" +default-control-database+) plain t)
    ;; create default database
    (must-succeed-query (format nil "CREATE DATABASE ~A" +default-database+) cryptdb)
    (must-succeed-query (format nil "CREATE DATABASE ~A" +default-control-database+) plain)
    ;; set proper default database
    (must-succeed-query (format nil "USE ~A" +default-database+) cryptdb)
    (must-succeed-query (format nil "USE ~A" +default-control-database+) plain)
    ;; begin testing
    (dolist (test-group all-test-groups results)
      (push (run-test-group connections test-group) results))
    (setf results (reverse results))))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;   commence cryptdb testing
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun main ()
  (let ((tests (load-tests)))
    (run-tests tests)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;        unit tests
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; FIXME: add tests for unspecified stuff
(defun test-fixup-onion-checks ()
       ;; a fully specifed check should be returned as is
  (and (let ((line '(:update ("database0" ("table0" ("field0" ("onion0" "seclevel0")
                                                              ("onion1" "seclevel1"))
                                                    ("field1" ("onion2" "seclevel2")))))))
         (equal line (fixup-onion-checks line nil)))
       ;; a single check does not require nesting
       (equal '(:update ("database" ("table" ("field" ("onion" "seclevel")))))
              (fixup-onion-checks '(:update "database" "table" "field" "onion" "seclevel") nil))
       (null (fixup-onion-checks '(:update ()) nil))
       (null (fixup-onion-checks :update nil))
       (equal '(:check)
              (fixup-onion-checks '(:check) nil))
       (equal '(:check)
              (fixup-onion-checks :check nil))
       (null (fixup-onion-checks  :all-max nil))
       (equal '(:all-max "db" "table")
              (fixup-onion-checks '(:all-max "db" "table") nil))
       ;; use hardcoded default database
       (equal `(:all-max ,+default-database+ "table")
              (fixup-onion-checks '(:all-max "table") t))
       (equal `(:update (,+default-database+ ("table" ("field" ("onion" "seclevel")))))
              (fixup-onion-checks '(:update "table" "field" "onion" "seclevel") t))
       ;; use default database from test group
       (equal '(:all-max "some-default" "table")
              (fixup-onion-checks '(:all-max "table") "some-default"))
       (equal '(:update ("a-default" ("table" ("field" ("onion" "seclevel")))))
              (fixup-onion-checks '(:update "table" "field" "onion" "seclevel") "a-default"))))

(defun test-fixup-onion-checks-multiple-update-default-bug ()
  (let ((line '(:update ("t" ("f"  ("o"  "l"))
                             ("f2" ("o2" "l2"))))))
    (fixup-onion-checks line t)))

(defun test-fixup-execution-target ()
  (and (eq :both (fixup-execution-target :both))
       (eq :both (fixup-execution-target nil))
       (eq :control (fixup-execution-target :control))
       (eq :cryptdb (fixup-execution-target :cryptdb))
       (eq nil (fixup-execution-target 'whatever))))

(defun test-lookup-seclevel ()
  (let ((onions
          (make-onion-state
            :databases '(("database"
                            ("table0" ("field0" ("onion0" "level")))
                            ("table1" ("field1" ("onion1" "anotherlevel")
                                                ("onion2" "moremore"))
                                      ("field2" ("onion3" "again"))))))))
    (and (string= "level"        (lookup-seclevel onions "database" "table0" "field0" "onion0"))
         (string= "anotherlevel" (lookup-seclevel onions "database" "table1" "field1" "onion1"))
         (string= "moremore"     (lookup-seclevel onions "database" "table1" "field1" "onion2"))
         (string= "again"        (lookup-seclevel onions "database" "table1" "field2" "onion3")))))

(defun test-update-onion-state! ()
  (let ((onions
          (make-onion-state
            :databases '(("database"
                            ("table0" ("field0" ("onion0" "level")))
                            ("table1" ("field1" ("onion1" "anotherlevel")
                                                ("onion2" "moremore"))
                                      ("field2" ("onion3" "again"))))))))
    (update-onion-state! onions '(:update ("database" ("table1" ("field1" ("onion1" "newlevel"))))))
    (string= "newlevel"
             (lookup-seclevel onions "database" "table1" "field1" "onion1"))))

#|
(defun test-multiple-update-onion-state! ()
  (let ((onions
          (make-onion-state
            :databases '(("d" ("t" ("f"  ("o"  "l"))
                                   ("f2" ("o2" "l2"))))))))
    (update-onion-state! onions '(:update ("d" 
  nil)
|#

(defun test-many-str-assoc ()
  (and (multiple-value-bind (success lookup unmatched-keys)
            (many-str-assoc '(a b c) '((a (b (x y)
                                             (c d)))))
         (and success
              (equal '(c d) lookup)
              (null unmatched-keys)))
       (let ((test (copy-list
                     '((a (b (c d)
                             (e f))
                          (c (c d)))))))
         (multiple-value-bind (success lookup unmatched-keys)
            (many-str-assoc '(a c c) test)
           (and success
                (equal '(c d) lookup)
                (null unmatched-keys)
                (multiple-value-bind (success-2 lookup-2 unmatched-keys-2)
                    (many-str-assoc '(a c c) test)
                  (declare (ignore unmatched-keys-2))
                  (assert success-2)
                  (setf (cdr lookup-2) '(g))
                  (equal '((a (b (c d)
                                 (e f))
                              (c (c g))))
                         test)))))
       (multiple-value-bind (success lookup unmatched-keys)
            (many-str-assoc '(x y z) '((x (y)
                                          (z))))
         (and (not success)
              (equal '(y) lookup)
              (equal '(z) unmatched-keys)))
       (multiple-value-bind (success lookup unmatched-keys)
            (many-str-assoc '(x y z)  '((x (y (q)
                                              (p)
                                              (r)))))
         (and (not success)
              (equal '(y (q)
                         (p)
                         (r))
                     lookup)
              (equal '(z) unmatched-keys)))
       (multiple-value-bind (success lookup unmatched-keys)
            (many-str-assoc '(a b c) '((x (y))))
         (and (not success)
              (equal '((x (y))) lookup)
              (equal '(a b c) unmatched-keys)))))

(defun test-add-onion! ()
  (let ((onions (make-onion-state)))
    (add-onion! onions "db0" "t0" "f0" "o0" "l0")
    (and (equal '(("db0" ("t0" ("f0" ("o0" "l0")))))
                (onion-state-databases onions))
         (string= "l0" (lookup-seclevel onions "db0" "t0" "f0" "o0"))
         (progn
           (add-onion! onions "db0" "t0" "f1" "o1" "l1")
           (and (string= "l0" (lookup-seclevel onions "db0" "t0" "f0" "o0"))
                (string= "l1" (lookup-seclevel onions "db0" "t0" "f1" "o1"))))
         (progn
           (add-onion! onions "db0" "t1" "f2" "o1" "l2")
           (and (string= "l0" (lookup-seclevel onions "db0" "t0" "f0" "o0"))
                (string= "l1" (lookup-seclevel onions "db0" "t0" "f1" "o1"))
                (string= "l2" (lookup-seclevel onions "db0" "t1" "f2" "o1")))))))

(defun test-map-tree ()
  (and (equal '((2 3) (4 ((5))) 6)
              (map-tree #'(lambda (n) (1+ n)) '((1 2) (3 ((4))) 5)))
       (equal '()
              (map-tree #'(lambda (n) (declare (ignore n)) (assert nil)) '()))))

(defun test-list-depth ()
  (and (= 1 (list-depth '()))
       (= 1 (list-depth '(2 3 4)))
       (= 2 (list-depth '(1 (2) (4) 7)))
       (= 3 (list-depth '(1 (2) 3 4 (5 (6) 7))))))

(defun test-all-units ()
  (and (test-fixup-onion-checks)
       (test-fixup-onion-checks-multiple-update-default-bug)
       (test-fixup-execution-target)
       (test-lookup-seclevel)
       (test-update-onion-state!)
       (test-many-str-assoc)
       (test-add-onion!)
       (test-map-tree)
       (test-list-depth)))


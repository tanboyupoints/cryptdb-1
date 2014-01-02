(proclaim '(optimize (debug 3)))

; (defconstant +default-tests-path+ "./tests.sexpr")
(defparameter +default-tests-path+  "./tests.sexpr")
(defparameter +default-ip+          "127.0.0.1")
(defparameter +default-username+    "root")
(defparameter +default-password+    "letmein")

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
    (sql-database-error (e)
      (declare (ignore e))
      'query-error)))


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
  (assoc key alist :test #'string=))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;   handle test file input
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun load-tests (&optional (path +default-tests-path+))
  (read-file path))

;; FIXME: implement
(defun proper-onion-update? (onion-check)
  (assert (listp onion-check))
  t)

;;; take possibly shorthand onion-checks and produce full length checks
;;; > return nil if `dirty-onion-checks` are invalid
(defun fixup-onion-checks (dirty-onion-checks)
  (cond ((atom dirty-onion-checks)
         (when (eq dirty-onion-checks :check)
           `(,dirty-onion-checks)))
        ((eq :check (car dirty-onion-checks))
         (when (= 1 (length dirty-onion-checks))
           dirty-onion-checks))
        ((eq :all-max (car dirty-onion-checks))
         (when (and (= 3 (length dirty-onion-checks))
                    (every #'atom dirty-onion-checks))
           dirty-onion-checks))
        ((eq :update (car dirty-onion-checks))
         (cond ((and (every #'atom dirty-onion-checks)
                     (= 5 (length (cdr dirty-onion-checks))))
                `(:update (,(second dirty-onion-checks)             ;; database
                           (,(third dirty-onion-checks)             ;; table
                            (,(fourth dirty-onion-checks)           ;; field
                             (,(fifth dirty-onion-checks)           ;; onion
                              ,(sixth dirty-onion-checks)))))))     ;; seclevel
               ((every #'proper-onion-update? (cdr dirty-onion-checks))
                dirty-onion-checks)))))

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

(defun max-level? (onion seclevel)
  (cond ((member onion '("DET" "OPE") :test #'string=)
         (string= "RND" seclevel))
        ((string= "AGG" onion) (string= "HOM" seclevel))))

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
          (output       nil))
      (dolist (row results output)
        (destructuring-bind (database table field onion seclevel) row
          (unless (or (not (string= max-database database))
                      (not (string= max-table table))
                      (max-level? onion seclevel))
            (setf output nil))
          ;; update our local copy of onion state
          (setf (lookup-seclevel onions database table field onion) seclevel)))))
  (:method (connections (onions onion-state) (type (eql :update)) onion-check)
    (update-onion-state! onions onion-check)
    (handle-check connections onions :check onion-check))
  (:method (connections (onions onion-state) (type (eql :check)) onion-check)
    (let ((results (get-cryptdb-show connections))
          (output  t))
      (dolist (row results output)
        (destructuring-bind (database table field onion seclevel) row
          (unless (string= seclevel (lookup-seclevel onions database table field onion))
            (setf output nil))
          (setf (lookup-seclevel onions database table field onion) seclevel))))))

;;; take the per query onion checks and use them to update our onion state;
;;; then compare this new onion state to the state reported by cryptdb for
;;; each testing onion
(defmethod handle-onion-checks (connections (onions onion-state) onion-check)
  (handle-check connections onions (car onion-check) onion-check))


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
  (error "implement slow-compare"))

(defun compare-results (results-a results-b)
  (or (fast-compare results-a results-b) (slow-compare results-a results-b)))

(defun run-test-group (connections test-group)
  (let* ((score (make-group-score))
         (*score* score)
         (onions (make-onion-state)))
    (declare (special *score*))
    (dolist (test-case test-group score)
      (let* ((query (car test-case))
             (onion-checks (fixup-onion-checks (cadr test-case)))
             (execution-target (fixup-execution-target (caddr test-case)))
             (cryptdb (connection-state-cryptdb connections))
             (control (connection-state-plain connections)))
        (case execution-target
          (:cryptdb (update-score (issue-query query cryptdb t)))
          (:control (update-score (issue-query query control t)))
          (:both
            (let ((cryptdb-results
                    (clsql:query query :database (connection-state-cryptdb connections)))
                  (plain-results
                    (clsql:query query :database (connection-state-plain connections))))
              (update-score
                (and (compare-results cryptdb-results plain-results)
                     (handle-onion-checks connections onions onion-checks)))))
          (otherwise (error "unknown execution-target!")))))))

(defun run-tests (all-test-groups)
  (let ((connections
          (make-connection-state :cryptdb (db-connect nil 3307)
                                 :plain   (db-connect nil 3306)))
        (results '()))
    (dolist (test-group all-test-groups results)
      (push (run-test-group connections test-group) results))
    (setf results (reverse results))))


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
                                                    ("field1" (("onion2" "seclevel2"))))))))
         (equal line (fixup-onion-checks line)))
       ;; a single check does not require nesting
       (equal '(:update ("database" ("table" ("field" ("onion" "seclevel")))))
              (fixup-onion-checks '(:update "database" "table" "field" "onion" "seclevel")))
       (equal '(:update ())
              (fixup-onion-checks '(:update ())))
       (null (fixup-onion-checks  :update))
       (equal '(:check)
              (fixup-onion-checks '(:check)))
       (equal '(:check)
              (fixup-onion-checks :check))
       (null (fixup-onion-checks  :all-max))
       (equal '(:all-max "db" "table")
              (fixup-onion-checks '(:all-max "db" "table")))))

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

(defun test-all-units ()
  (and (test-fixup-onion-checks)
       (test-fixup-execution-target)
       (test-lookup-seclevel)
       (test-update-onion-state!)))


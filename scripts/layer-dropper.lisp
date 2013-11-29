;;;; this code will generate a common lisp file that will replay the
;;;; onion adjustments that have occurred to ``*database*''
(defparameter *database* "ezyang+blog")

(defun verify-fields (fields)
  (and (string= (nth 0 fields) "_database")
       (string= (nth 1 fields) "_table")
       (string= (nth 2 fields) "_field")
       (string= (nth 3 fields) "_onion")
       (string= (nth 4 fields) "_level")
       (string= (nth 5 fields) "id")))

(defun generate-dropper (row)
  (assert (= 6 (length row)))
  `(clsql:query ,(apply #'format nil "SET @cryptdb='adjust',
                                          @database='~A',
                                          @table='~A',
                                          @field='~A',
                                          @~A='~A'"
                                 (butlast row))))

(defun do-stuff (database)
  "build a lisp form for adjusting ``database''"
  (let ((c (clsql:connect
             '("127.0.0.1" "burrows+ezcrypted" "root" "letmein" 3307)
              :database-type :mysql
              :if-exists :new)))
    (unwind-protect
      (multiple-value-bind (onions fields)
          (clsql:query "SET @cryptdb='show'")
        (assert (verify-fields fields))
        ;;; remove RND and HOM onions
        (setf onions
              (remove-if #'(lambda (row)
                             (member (nth 4 row)
                                     '("RND" "HOM")
                                     :test #'string=))
                         onions))
        ;;; only use results pertaining to 'database'
        (setf onions
              (remove-if-not #'(lambda (row)
                                 (string= (nth 0 row) database))
                             onions))
        ;;; build the output lisp forms
        `(defun adjust-main ()
            ,(format nil "onion adjuster for database: ~A" database)
            ,@(mapcar #'generate-dropper onions)
            nil))
      (clsql:disconnect :database c))))

(defun main ()
  (with-open-file (stream "onions.lisp" :direction :output
                                        :if-exists :supersede)
    (pprint (do-stuff *database*) stream)))

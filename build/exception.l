;;;
;;; exception-case macro
;;;
(:defcon exception-case (:macro (sym form :rest forms)
  (if (null (symbolp sym))
      (raise "exception-case: is not a symbol" sym)
      (let ((typesym (gensym)))
        (list* (list 'let (list (list typesym (list 'type-of keyform)))
                     (list* 'cond 
                            (mapcar-1
                             (:lambda (clause)
                                      (if (consp clause)
                                          (list* (list 'eq (car clause) typesym) (cdr clause))
                                        (raise "exception-case: illegal clause " forms)))
                             type-clauses))))))))

#|
(defun must-not-be-nil (thing)
  (if (null thing)
    (raise-exception (exception :nil "must not be :nil" thing))
    thing))

(exception-case ex (must-not-be-nil :nil)
  (:nil (let ((thing (svref ex 4)))
           (fmt :t "~A can't be nil~%" thing)
           (break (cd ex))))
  (:t (fmt :t "some other exception ~A~%" (cdr ex))))

;;; =>

(with-exception (:lambda () (must-not-be-nil :nil))
                (:lambda (ex)
                   (let* ((ex-view (view ex))
                          (tag (svref ex-view 0))
                          (reason (svref ex-view 1))
                          (thing (svref ex-view 2)))
                     (cond
                       ((eq tag :nil) 
                          (let ((thing (svref (view (cdr ex)) 3)))
           (fmt :t "in ~A, ~A can't be nil~%" (car ex) thing)
           :nil))
  (:t 
  )

|#

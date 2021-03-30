;;;
;;; condition-case macro
;;;
(:defcon condition-case (:macro (sym form :rest forms)
  (if (null (symbolp sym))
      (raise "condition-case: is not a symbol" sym)
      (let ((typesym (gensym)))
        (list* (list 'let (list (list typesym (list 'type-of keyform)))
                     (list* 'cond 
                            (mapcar-1
                             (:lambda (clause)
                                      (if (consp clause)
                                          (list* (list 'eq (car clause) typesym) (cdr clause))
                                        (raise "condition-case: illegal clause " forms)))
                             type-clauses))))))))

#|
(defun must-not-be-nil (thing)
  (if (null thing)
    (raise-condition (condition :nil "must not be :nil" thing))
    thing))

(condition-case ex (must-not-be-nil :nil)
  (:nil (let ((thing (svref ex 4)))
           (fmt :t "~A can't be nil~%" thing)
           (break (cd ex))))
  (:t (fmt :t "some other condition ~A~%" (cdr ex))))

;;; =>

(with-condition (:lambda () (must-not-be-nil :nil))
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

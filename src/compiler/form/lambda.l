;;;
;;; lambda compiler
;;;
(defun compiler:lambda (llist body)
  (let ((rewrite (:lambda (llist rest body))
                 (fmt :t "rewriting symbols ~A,~A in ~A~%" llist rest body)))
    (and (not (listp llist)) (raise "lambda: lambda list is not a list" llist))
    (and (not (listp body)) (raise "lambda: body is not a list" body))
    (cond
     ((listp llist)
      ;; check for non-symbols
      (mapc
       (:lambda (sym)
                (and (not (symbolp sym))
                     (raise "lambda: lambda list element is not a symbol" sym)))
        llist)
      ;; check for duplicates
      #|
      (mapl
       (:lambda (llist)
                (and (not (null (list-find (car llist) (cdr llist))))
                     (raise "lambda: duplicate symbol in lambda list" (car llist))))
       llist)
      |#
      ;; get rest symbol and rewrite body
      (mapl
       (:lambda (llist)
                (if (eq (car llist) :rest)
                    (let ((rest (nth 1 llist)))
                      (and (null rest) (raise "lambda: no rest symbol" :nil))
                      (rewrite llist rest body))
                  :nil))
       llist))
   (:t (raise "lambda: is not a lambda list" llist)))))

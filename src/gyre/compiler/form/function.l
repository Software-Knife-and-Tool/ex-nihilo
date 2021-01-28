;;;
;;; function/closure compiler
;;;
(defmacro s-function (form)
  (cond
   ((symbolp form)
    (if (functionp (symbol-value form))
        (symbol-value form)
        (raise "function: is not a function symbol" form)))
   ((consp form)
    (if (eq (car form) (keyword "lambda"))
        (gyre:lambda form)
        (raise "function: is not a lambda form" form)))
   (:t  (raise "function: is not a function symbol or lambda form" form))))

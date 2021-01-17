;;;
;;; function/closure compiler
;;;
(defmacro s-function (form)
  (cond
   ((symbol? form)
    (if (function? (symbol-value form))
        (symbol-value form)
        (raise "function: is not a function symbol" form)))
   ((cons? form)
    (if (eq (car form) (keyword "lambda"))
        (lambda form)
        (raise "function: is not a lambda form" form)))
   (:t  (raise "function: is not a function symbol or lambda form" form))))

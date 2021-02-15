;;;
;;; code reduction
;;;

(defmacro vfmt (:rest args)
  (when (boundp '%verbose%)
    (list '(list* 'mu:apply args))))

(defun constant-fold (fn args)
  (let ((constantp (:lambda (form)
          (unless (or (symbolp form) (consp form)) :t)))
        (folds
         (list
          (cons 'list list)
          (cons 'fixnum+ fixnum+)
          (cons 'fixnum- fixnum-)
          (cons 'fixnum* fixnum*)
          (cons 'fixnum/ fixnum/)
          (cons 'fixnum< fixnum<))))

    (when (foldl
           (:lambda (acc el) (and (constantp el) acc))
           :t
           args)
      (let ((fnc (assq fn folds)))
        (when fnc (mu:apply fnc args))))))

(defun compiler:reduce (sexpr)
  (let ((funcall
        (:lambda (form)
          (let ((fn (car form))
                (args (cdr form)))
            (cond
             ((macro-function fn)
              (fmt :t ";;; reduce:funcall:macro: ~A~%" fn)
              (opt-expr (macroexpand form)))
             ((functionp fn)
              (fmt :t ";;; reduce:funcall:function: ~A~%" fn)
              (list* fn (mapcar-1 opt-expr args)))
             ((consp fn)
              (fmt :t ";;; reduce::funcall:cons: ~A~%" fn)
              form)
             ((and (symbolp fn) (special-operatorp fn))
              (fmt :t ";;; reduce:funcall:special-operator: ~A~%" fn)
              (cond
               ((eq fn :quote)
                (let ((obj (car args)))
                  (if (or (symbolp obj) (consp obj))
                    form
                    (progn (fmt :t ";;;    quote reduction: ~A~%" obj) obj))))
               (list :t form)))
             ((symbolp fn)
              (fmt :t ";;; reduce:funcall:symbol: ~A~%" fn)
              (let ((fold (constant-fold fn (mapcar-1 opt-expr args))))
                (cond
                 ((null fold)
                    (let ((fnc (symbol-value fn)))
                      (unless (functionp fnc) (fmt :t "~A is not bound to a function~%" fn))
                      (fmt :t ";;;    function symbol substitution: ~S~%" fnc)
                      (list* fnc fold)))
                  (:t (fmt :t ";;;    constant fold: ~A~%" fold) fold)))) 
             (:t (raise "unimplemented funcall form" fn)))))))

    (cond
     ((or (null sexpr) (atom sexpr))
      (fmt :t ";;; reduce::atom ~A~%" sexpr)
      (cond
       ((symbolp sexpr)
        (if (boundp sexpr)
          (let ((value (symbol-value sexpr)))
            (progn (fmt :t ";;;    bound symbol substitution: ~A~%" value) value))
          sexpr))
      (:t sexpr)))
     (:t (funcall sexpr)))))


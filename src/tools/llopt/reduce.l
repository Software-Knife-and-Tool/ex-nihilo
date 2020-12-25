;;;
;;; code reduction
;;;
(load-once llopt/rewrite "/tools/llopt/rewrite.l")

(defmacro vfmt (:rest args)
  (when (bound? '%verbose%)
    (list '(list* 'apply-1 args))))

(defun constant-fold (fn args)
  (let ((constantp (:lambda (form)
          (unless (or (symbol? form) (cons? form)) :t)))
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
      (let ((fnc (assoc fn folds)))
        (when fnc (apply-1 fnc args))))))

(defun reduce (sexpr)
  (let ((funcall
        (:lambda (form)
          (let ((fn (car form))
                (args (cdr form)))
            (cond
             ((macro-function fn)
              (fmt :t ";;; reduce:funcall:macro: ~A~%" fn)
              (opt-expr (macroexpand form)))
             ((function? fn)
              (fmt :t ";;; reduce:funcall:function: ~A~%" fn)
              (list* fn (mapcar-1 opt-expr args)))
             ((cons? fn)
              (fmt :t ";;; reduce::funcall:cons: ~A~%" fn)
              form)
             ((and (symbol? fn) (special-operator? fn))
              (fmt :t ";;; reduce:funcall:special-operator: ~A~%" fn)
              (cond
               ((eq fn :quote)
                (let ((obj (car args)))
                  (if (or (symbol? obj) (cons? obj))
                    form
                    (progn (fmt :t ";;;    quote reduction: ~A~%" obj) obj))))
               (:t form)))
             ((symbol? fn)
              (fmt :t ";;; reduce:funcall:symbol: ~A~%" fn)
              (let ((fold (constant-fold fn (mapcar-1 opt-expr args))))
                (cond
                 ((null fold)
                    (let ((fnc (symbol-value fn)))
                      (unless (function? fnc) (fmt :t "~A is not bound to a function~%" fn))
                      (fmt :t ";;;    function symbol substitution: ~S~%" fnc)
                      (list* fnc fold)))
                  (:t (progn (fmt :t ";;;    constant fold: ~A~%" fold) fold))))) 
             (:t (error "unimplemented funcall form" fn)))))))

    (cond
     ((or (null sexpr) (atom sexpr))
      (fmt :t ";;; reduce::atom ~A~%" sexpr)
      (cond
       ((symbol? sexpr)
        (if (bound? sexpr)
          (let ((value (symbol-value sexpr)))
            (progn (fmt :t ";;;    bound symbol substitution: ~A~%" value) value))
          sexpr))
      (:t sexpr)))
     (:t (funcall sexpr)))))

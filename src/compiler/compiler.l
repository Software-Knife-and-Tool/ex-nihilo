;;;
;;; compiler
;;;
(defconstant compiler::version "0.0.1")

(defun compile (sexpr)
  (let* ((special
          (:lambda (fn form)
            (cond
             ((eq fn :defcon)
              (compile (car form))
              (mapc-1 compile (cdr form)))
             ((eq fn :quote) (compile (car form)))
             ((eq fn :lambda) (fmt :t "special:lambda: ~A ~A~%" (car form) (cdr form)))
             ((eq fn :letq) (fmt :t "special:letq: ~A ~A~%" (car form) (cdr form)))
             ((eq fn :macro) (fmt :t "special:macro: ~A ~A~%" (car form) (cdr form)))
             (:t (error "special: what is you?" expr)))))
         (atomf
          (:lambda (expr)
            (cond
             ((null expr) expr)
             ((eq expr :t) expr)
             ((keyword? expr) (when (special-operator? expr) expr))
             ((symbol? expr) expr)
             ((fixnum? expr) expr)
             ((float? expr) expr)
             ((string? expr) expr)
             ((vector? expr) expr)
             (:t (error "atom: what is you?" expr)))))
         (funcall
            (:lambda (form)
              (let ((fn (car form))
                    (args (cdr form)))
                (cond
                 ((and (symbol? fn) (special-operator? fn))
                  (fmt :t "funcall:special-operator: (~A . ~A)~%" fn args)
                  (special fn form))
                 ((macro? fn) (fmt :t "funcall:macro: (~A . ~A)~%" fn args)
                  (compile fn)
                  (mapc-1 compile args))
                 ((symbol? fn)
                  (fmt :t "funcall:symbol: (~A . ~A)~%" fn args)
                  (unless (codegen fn args) (compile fn) (mapc-1 compile args)))
                 ((cons? fn) (fmt :t "funcall:cons: (~A . ~A)~%" fn args)
                  (compile fn)
                  (mapc-1 compile args))
                 ((function? fn) (fmt :t "funcall:function: (~A . ~A)~%" fn args)
                  (compile fn)
                  (mapc-1 compile args)))))))
    
    (cond
      ((or (null sexpr) (atom sexpr)) (atomf sexpr))
      (:t (funcall sexpr)))))
    
(defun print-sexpr (sexpr level)
  (let* ((spaces
          (:lambda (l)
            (cond
             ((eq l 0) "")
             ((eq l 1) " ")
             ((eq l 2) "  ")
             ((eq l 3) "   ")
             ((eq l 4) "    ")
             ((eq l 5) "     ")
             ((eq l 6) "      ")
             ((eq l 7) "       ")
             ((eq l 8) "        ")
             ((eq l 9) "         ")
             (:t ""))))
         (special
          (:lambda (fn form)
            (cond
             ((eq fn :defcon)
              (fmt :t "special:defcon: (~A ~A)~%" (car form) (cdr form))
              (print-sexpr (car form) (1+ level))
              (mapc-1 (:lambda (form) (print-sexpr form (1+ level))) (cdr form)))
             ((eq fn :quote)
              (fmt :t "special:quote: (~A . ~A)~%" form)
              (print-sexpr (car form) (1+ level)))
             ((eq fn :lambda) (fmt :t "special:lambda: ~A ~A~%" (car form) (cdr form)))
             ((eq fn :letq) (fmt :t "special:letq: ~A ~A~%" (car form) (cdr form)))
             ((eq fn :macro) (fmt :t "special:macro: ~A ~A~%" (car form) (cdr form))))))
         (atomf
          (:lambda (expr)
            (cond
             ((null expr) (fmt :t "atom:nil~%") expr)
             ((eq expr :t) (fmt :t "atom:t~%") expr)
             ((keyword? expr)
              (fmt :t "atom:keyword ~A~%" expr)
              (when (special-operator? expr) (fmt :t "special-operator: ~A~%")))
             ((symbol? expr) (fmt :t "atom:symbol ~A~%" expr))
             ((fixnum? expr) (fmt :t "atom:fixnum ~A~%" expr))
             ((float? expr) (fmt :t "atom:float ~A~%" expr))
             ((string? expr) (fmt :t "atom:string ~A~%" expr))
             ((vector? expr) (fmt :t "atom:vector ~A~%" expr))
             ((function? expr) (fmt :t "atom:function ~A~%" expr)))))
         (funcall
           (:lambda (form)
            (let ((fn (car form))
                  (args (cdr form)))
              (cond
               ((and (symbol? fn) (special-operator? fn))
                (fmt :t "funcall:special-operator: (~A . ~A)~%" fn args)
                (special fn form))
               ((macro? fn) (fmt :t "funcall:macro: (~A . ~A)~%" fn args)
                (print-sexpr fn (1+ level))
                (mapc-1 (:lambda (form) (print-sexpr form (1+ level))) args))
               ((symbol? fn)
                (fmt :t "funcall:symbol: (~A . ~A)~%" fn args)
                (print-sexpr fn (1+ level))
                (mapc-1 (:lambda (form) (print-sexpr form (1+ level))) args))
               ((cons? fn) (fmt :t "funcall:cons: (~A . ~A)~%" fn args)
                (print-sexpr fn (1+ level))
                (mapc-1 (:lambda (form) (print-sexpr form (1+ level))) (cdr form)))
               ((function? fn) (fmt :t "funcall:function: (~A . ~A)~%" fn args)
                (print-sexpr fn (1+ level))
                (mapc-1 (:lambda (form) (print-sexpr form (1+ level))) args)))))))

    (cond
      ((or (null sexpr) (atom sexpr)) (atomf sexpr))
      (:t (funcall sexpr)))))

;;;
;;; compile-file
;;;
(defun compile-file (file-name)
  (block :nil
    (unless (string? file-name)
      (fmt :t "~A is not a file path~%" file-name)
      (return :nil))
    (let ((src (make-input-file file-name)))
      (when (null src)
        (fmt :t "can't open ~A~%" file-name)
          (return :nil))
      (while (null (eof? src))
        (let ((sexpr (read src)))
          (compile sexpr))))))

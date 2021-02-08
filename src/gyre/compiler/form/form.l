;;;
;;; compiler
;;;
(defun compile-form (form)
  (let* ((specop
          (:lambda (name)
             (cond
              ((eql name ":defsym") :defsym)
              ((eql name ":lambda") :lambda)
              ((eql name ":letq") :letq)
              ((eql name ":macro") :macro)
              ((eql name ":nil") :nil)
              ((eql name ":quote") :quote)
              ((eql name ":t") :t)
              (:t :nil))))
         (macrof
          (:lambda (name) (macro-function (find-symbol (find-ns "mu") name))))
         (symbolf
          (:lambda (name) (find-symbol (find-ns "mu") name)))
         (special
          (:lambda (fn form)
             (let ((fn (specop fn)))
               (cond
                ((eq fn :defsym) (fmt :t "special:defsym: ~A%" form))
                ((eq fn :lambda) (fmt :t "special:lambda: ~A~%" form))
                ((eq fn :letq) (fmt :t "special:letq: ~A~%" form))
                ((eq fn :macro) (fmt :t "special:macro: ~A~%" form))
                ((eq fn :nil) (fmt :t "special:nil: ~A~%" form))
                ((eq fn :quote) (fmt :t "special:quote: ~A~%" form))
                ((eq fn :t) (fmt :t "special:t: ~A~%" form))
                (:t (error "special: what is you?" fn))))))
         (atomf
          (:lambda (expr)
            (cond
             ((null expr) expr)
             ((eq expr :t) expr)
             ((keywordp expr) (when (special-operatorp expr) expr))
             ((symbolp expr) expr)
             ((fixnump expr) expr)
             ((floatp expr) expr)
             ((stringp expr) expr)
             ((vectorp expr) expr)
             (:t (error "atom: what is youp" expr)))))
         (funcall
          (:lambda (form)
              ;;; (fmt :t "funcall: form ~A~%" form)
              (let ((fn (car form))
                    (args (cdr form)))
                (cond
                 ((and (consp fn) (eq :symbol (car fn)) (specop (cdr fn)))
                  (fmt :t "funcall:special-operator: (~A . ~A)~%" fn args)
                  (special (cdr fn) form))
                 ((and (consp fn) (eq :symbol (car fn)) (macrof (cdr fn)))
                  (fmt :t "funcall:macro: (~A . ~A)~%" fn args)
                  (compile-form fn)
                  (mapc compile-form args))
                 ((and (consp fn) (eq :symbol (car fn)) (symbolf (cdr fn)))
                  (fmt :t "funcall:symbol: (~A . ~A)~%" fn args)
                  (compile-form fn)
                  (mapc compile-form args))
                 ((and (consp fn) (listp (car fn)))
                  (fmt :t "funcall:cons: (~A . ~A)~%" fn args)
                  (compile-form fn)
                  (mapc compile-form args))
                 ((and (consp fn) (eq :func (car fn)))
                  (fmt :t "funcall:function: (~A . ~A)~%" fn args)
                  (compile-form fn)
                  (mapc compile-form args)))))))
    
    (cond
      ((or (null form) (atom form)) (atomf form))
      (:t (funcall form)))))

;;;
;;; llc main
;;;
(in-ns (ns "llc" (ns-current)))

(defconstant llc-version "0.0.2")

(load-once llc/special "/tools//llc/special.l")
(load-once llc/opencode "/tools//llc/codegen-x64.l")
(load-once llc/compile "/tools/llc/compile.l")

(defun llc (arg-string)
  (let ((args (read (make-input-string (fmt :nil "(~A)" arg-string)))))
    (fmt :t "llc ~A: args ~A~%" llc-version args)
    (block :nil
      (while (null (eof? :t))
        (with-exception (:lambda ()
          (let ((sexpr (progn (fmt :t "[llc] ~A> " (ns-name (ns-current))) (read :t))))
            (cond
             ((eq sexpr :?)
              (fmt :t ":? - this message~%")
              (fmt :t ":d - turn on debugging~%")
              (fmt :t ":p - parse expr~%")
              (fmt :t ":c - compile expr~%")
              (fmt :t ":f - compile file~%")
              (fmt :t ":q - exit~%"))
             ((eq sexpr :d)
              (unless (bound? '*debug*)
                (fmt :t ";;; debugging on~%")
                (defconstant *debug* :t)))
             ((eq sexpr :f) (fmt :t "~S~%" (compile-file (read :t))))
             ((eq sexpr :c) (fmt :t "~S~%" (compile (read :t))))
             ((eq sexpr :p) (fmt :t "~S~%" (print-sexpr (read :t) 0)))
             ((eq sexpr :q) (return :t))
             (:t (fmt :t "~A~%" (eval sexpr))))))
          break)))))



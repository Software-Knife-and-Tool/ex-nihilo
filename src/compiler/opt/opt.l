;;;
;;; opt main
;;;
(defconstant llopt-version "0.0.5")

(defun opt-expr (sexpr)
  (let* ((rw (rewrite sexpr))
         (rexpr (compiler:reduce rw)))
    (fmt :t ";;; opt-expr ~S~%    ~S~%" sexpr rexpr)
    rexpr))

(defun opt-file (file-name)
  (block :nil
    (unless (string? file-name)
      (fmt :t "~A is not a file path (llopt/opt-file)~%" file-name)
      (return :nil))
    (let ((src (make-input-file file-name)))
      (when (null src)
        (fmt :t "can't open ~A (llopt/opt-file)~%" file-name)
          (return :nil))
      (while (null (eof? src))
        (opt-expr (read src))))))

(defun llopt (arg-string)
  (let ((args (read (make-input-string (fmt :nil "(~A)" arg-string)))))
    (fmt :t "llopt ~A: args ~A~%" llopt-version args)
    (block :nil
      (while (null (eof? :t))
        (with-exception (:lambda ()
          (let ((sexpr (progn (fmt :t "[llopt] ~A> " (ns-name (current-ns))) (read :t))))
            (cond
             ((eq sexpr :?)
              (fmt :t ":? - this message~%")
              (fmt :t ":f - optimize file~%")
              (fmt :t ":t - optimize test.l~%")
              (fmt :t ":r - optimize expr~%")
              (fmt :t ":v - set verbose~%")
              (fmt :t ":q - exit~%"))
             ((eq sexpr :f)
              (fmt :t "~S~%" (opt-file (read :t))))
             ((eq sexpr :r)
              (let ((sexpr (read :t)))
                (fmt :t ";;; optimize ~A~%~S~%" sexpr (opt-expr sexpr))))
             ((eq sexpr :t)
              (fmt :t "~S~%" (opt-file "test.l")))
             ((eq sexpr :v)
              (unless (bound? '%verbose%)
                (fmt :t ";;; verbose on~%")
                (defconstant %verbose% :t)))
             ((eq sexpr :q) (return :t))
             (:t (fmt :t "~A~%" (eval sexpr))))))
          break)))))


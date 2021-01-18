;;;
;;; compiler
;;;
(defconstant compiler::version "0.0.1")

;;;
;;; compile-file
;;;
(defun compile-file (src-path obj-path)
  (unless (stringp src-path) (raise "is not a file path" src-path))
  (unless (stringp obj-path) (raise "is not a file path" obj-path))
  (let ((src (make-input-file src-path))
        (obj (make-output-file obj-path)))
    (when (null src-path) (raise "can't open" src-path))
    (when (null obj-path) (raise "can't open" obj-path))
    (let ((ns (in-ns (ns (symbol-name (gensym)) (ns-current)))))
      (while (null (eofp src))
        (let ((sexpr (read src)))
          (compiler::compile-form sexpr))))))

;;;
;;; compiler
;;;
(defconstant compiler::version "0.0.1")

;;;
;;;
;;;
(defun open-tmp-file (prefix)
  (open-output-file (concatenate :string prefix ".tmp")))
          
;;;
;;; compile-file
;;;
(defun compile-file (src-path obj-path)
  (unless (stringp src-path) (raise "is not a file path" src-path))
  (unless (stringp obj-path) (raise "is not a file path" obj-path))
  (let ((src (open-input-file src-path))
        (obj (open-output-file obj-path)))
    (when (null src) (raise "compile-file: can't open" src-path))
    (when (null obj) (raise "compile-file: can't open" obj-path))
    (while (null (eofp src))
      (compile-form (ast src)))))

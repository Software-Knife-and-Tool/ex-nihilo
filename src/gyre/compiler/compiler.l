;;;
;;; compiler
;;;

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
    (emit obj
      (let ((ns (in-ns (ns (symbol-name (gensym)) (list (current-ns))))))
        (while (null (eofp src))
          (compile-form (ast src)))))))

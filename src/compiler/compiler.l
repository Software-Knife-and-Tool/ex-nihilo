;;;
;;; compiler
;;;
(defconstant compiler::version "0.0.1")

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

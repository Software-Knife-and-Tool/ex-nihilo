;;;
;;; x64 primitives
;;;

#|
;;; fixnum+ code generator
(defun cg-fixnum+ (args)
  (unless (eq (length args) 2) (error "requires 2 arguments (cg-fixnum+)" args))
  (let ((arg1 (nth 0 args))
        (arg2 (nth 1 args)))
    (cond
     ((and (typep arg1 :fixnum) (typep arg2 :fixnum))
      (fmt :t "    # fixnum+ ~A ~A~%" arg1 arg2)
      (fmt :t "    mov %rdi,~A~%" arg1)
      (fmt :t "    add %rdi,~A~%" arg2))
     (:t :nil))))
|#

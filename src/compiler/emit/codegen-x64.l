;;;
;;; x86-64 code generation
;;;
(defconstant primitives
  (list (cons 'fixnum+ cg-fixnum+)))

(defun opencode (form ostream)
  (check-type form :cons "is not a list (opencode)")
  (let ((out ostream))
    (check-type out :stream "is not a stream (opencode)")
    (let ((op (car form)))
      (when (null (find-if
                   (:lambda (el) (eq el op))
                   x86-ops))
        (raise "opcode not found" op))
      (fmt out "    ~A    ~A~%" op (cdr form)))))

(defun primitivep (fn)
  (assq fn primitives))

(defun codegen (fn args)
  (let ((cg (assq fn primitives)))
    (when cg (cg args))))

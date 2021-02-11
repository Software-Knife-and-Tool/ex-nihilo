;;;
;;; register model
;;;

(defstruct reg-model
  (assign (:lambda (reg &rest)) :func)
  (branch (:lambda (label-expr &rest args)) :func)
  (const (:lambda (const)) :func)
  (goto (:lambda (label-expr)) :func)
  (op (:lambda (fn)) :func)
  (perform (:lambda (op-expr)) :func)
  (restore (:lambda (reg-expr)) :func)
  (save (:lambda (reg-expr)) :func) 
  (test (:lambda (op-expr)) :func))


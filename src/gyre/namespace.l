;;;
;;; namespace functions
;;;

;;; map function onto namespace symbols
(defun with-ns-symbols (fn ns)
  (check-type fn :func "is not a function (with-ns-symbols)")
  (check-type ns :ns "is not a namespace (with-ns-symbols)")
  (mu:mapc fn (ns-symbols ns))
  ns)


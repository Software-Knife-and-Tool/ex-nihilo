;;;
;;; compiler library
;;;
(let ((ns (in-ns (ns "compiler" (find-ns "common")))))
  (require compiler "/src/compiler/compiler.l")
  (in-ns ns))

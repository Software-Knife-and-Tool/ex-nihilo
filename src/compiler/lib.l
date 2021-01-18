;;;
;;; compiler library
;;;
(let ((ns (in-ns (ns "compiler" (find-ns "common")))))
  (require compiler "/src/compiler/compiler.l")
  (require compiler/ast "/src/compiler/ast/lib.l")
;;;  (require compiler/emit "/src/compiler/emit/lib.l")
;;;  (require compiler/gyre "/src/compiler/form/lib.l")
;;;  (require compiler/opt "/src/compiler/opt/lib.l")
  (in-ns ns))

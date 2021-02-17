;;;
;;; compiler library
;;;
(require common "/src/common/lib.l")

(let ((ns (in-ns (ns "compiler" (list (find-ns "common"))))))
  (require compiler/ast "/src/compiler/ast/lib.l")
  (require compiler/form "/src/compiler/form/lib.l")
  (require compiler/compiler "/src/compiler/compiler.l")
  (require compiler/asm "/src/compiler/asm/lib.l")
  (require compiler/emit "/src/compiler/emit/lib.l")
;;;  (require compiler/opt "/src/compiler/opt/lib.l")
  (require compiler/opt "/src/compiler/reg-model/lib.l")
  (in-ns ns))

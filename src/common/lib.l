;;;
;;; common library
;;;
(let ((ns (in-ns (ns "common" (list (find-ns "core"))))))11
  (require common/common "/src/common/common.l")
  (require common/debug "/src/common/debug.l")
  (require common/list "/src/common/list.l")
  (require common/sequence "/src/common/sequence.l")
  (require common/stream "/src/common/stream.l")
  (in-ns ns))

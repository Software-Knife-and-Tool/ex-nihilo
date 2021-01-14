;;;
;;; common library
;;;
(in-ns (ns "canon" (find-ns "mu")))

(defconstant .version "0.0.1")

(load-once common/common "/src/common/common.l")
(load-once common/debug "/src/common/debug.l")
(load-once common/list "/src/common/list.l")
(load-once common/sequence "/src/common/sequence.l")
(load-once common/stream "/src/common/stream.l")

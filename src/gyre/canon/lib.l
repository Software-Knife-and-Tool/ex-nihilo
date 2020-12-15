;;;
;;; canon library
;;;
(in-ns (ns "canon" (find-ns "mu")))

(defconstant .version "0.0.18")

;;; load core first
(load-once canon/core "/canon/core/core.l")
(load-once canon/common "/canon/canon/common.l")
(load-once canon/common "/canon/canon/compiler.l")
(load-once canon/debug "/canon/canon/debug.l")
(load-once canon/list "/canon/canon/list.l")
(load-once canon/namespace "/canon/canon/namespace.l")
(load-once canon/repl "/canon/canon/repl.l")
(load-once canon/sequence "/canon/canon/sequence.l")
(load-once canon/stream "/canon/canon/stream.l")

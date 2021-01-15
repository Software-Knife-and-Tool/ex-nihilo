;;;
;;; gyre library
;;;
(require common "/src/common/lib.l")

(in-ns (ns "gyre" (find-ns "common")))

(defconstant gyre::version "0.0.1")

(require gyre/list "/src/gyre/list.l")
(require gyre/namespace "/src/gyre/namespace.l")
(require gyre/repl "/src/gyre/repl.l")
(require gyre/sequence "/src/gyre/sequence.l")
(require gyre/stream "/src/gyre/stream.l")

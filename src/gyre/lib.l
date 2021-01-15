;;;
;;; gyre library
;;;
(require common "/src/common/lib.l")

(let ((ns (in-ns (ns "gyre" (find-ns "common")))))
  (require gyre/list "/src/gyre/list.l")
  (require gyre/ns "/src/gyre/namespace.l")
  (require gyre/repl "/src/gyre/repl.l")
  (require gyre/stream "/src/gyre/stream.l")
  (in-ns ns))


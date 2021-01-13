#! /bin/bash
/usr/local/logica/bin/mu-exec                          \
    -l /usr/local/logica/canon/core/mu.l               \
    -q '(:defsym lib-base "/opt/gyre/src")'            \
    -q '(load-once gyre/canon "/core/gyre.l")'         \
    -q '(in-ns (ns "user" (find-ns "core")))'          \
    "$@"                                               \
    -e '(repl)'

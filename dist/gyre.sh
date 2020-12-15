#! /bin/bash
/usr/local/logica/bin/mu-repl                          \
    -l /usr/local/logica/canon/core/mu.l               \
    -q '(:defcon lib-base "/usr/local/logica")'        \
    -q '(load-once logica/canon "/canon/canon/lib.l")' \
    -q '(in-ns (ns "user" (find-ns "canon")))'         \
    "$@"                                               \
    -e '(repl)'

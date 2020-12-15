#! /bin/bash
/usr/local/logica/bin/mu-repl                      \
    -l /usr/local/logica/canon/core/mu.l           \
    -q '(:defcon lib-base "/usr/local/logica")'    \
    -q '(load-once canon/canon "/canon/canon/lib.l")' \
    "$@"


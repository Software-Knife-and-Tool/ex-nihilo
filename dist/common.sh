#! /bin/bash
/opt/gyre/bin/mu-exec                              \
    -l /opt/gyre/src/core/mu.l                     \
    -l /opt/gyre/src/core/core.l                   \
    -q '(:defsym lib-base "/opt/gyre")'            \
    -q '(require common "/src/common/lib.l")'      \
    "$@"                                           \
    -i

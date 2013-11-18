#!/bin/sh
ARGS=$@
rm -f vgdump
G_SLICE=always-malloc G_DEBUG=gc-friendly libtool --mode=execute valgrind --tool=memcheck --leak-check=full --leak-resolution=high --show-reachable=yes --num-callers=20 --log-file=vgdump ./waei $ARGS

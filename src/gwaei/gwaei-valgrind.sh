#!/bin/sh
ARGS=$@
rm -f vgdump
G_SLICE=always-malloc G_DEBUG=gc-friendly libtool --mode=execute valgrind --tool=memcheck --leak-check=full --leak-resolution=high --num-callers=30 --log-file=vgdump ./gwaei $ARGS

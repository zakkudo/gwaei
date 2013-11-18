#! /bin/sh

gnome-doc-prepare -c -f \
&& aclocal \
&& autoheader -f \
&& automake -c -f --add-missing \
&& autoconf -f

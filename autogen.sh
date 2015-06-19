#!/bin/sh

set -e

touch ChangeLog
libtoolize --ltdl --force
glib-gettextize --copy --force
intltoolize --copy --force --automake
gnome-doc-prepare --copy --force
gtkdocize
aclocal -I m4 --force
autoheader -f
automake --copy --force --add-missing
autoconf --force

#$gnome-autogen.sh

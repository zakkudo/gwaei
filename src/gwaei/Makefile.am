SUBDIRS = include/gwaei help mingw

PACKAGE = @PACKAGE@
VERSION = @VERSION@
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = gwaei 

datadir = @datadir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DLIBDIR=\"$(libdir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\"

gwaei_SOURCES = gwaei.c application.c application-callbacks.c mainwindow.c mainwindow-callbacks.c

gwaei_LDADD =  $(GWAEI_LIBS) ../libwaei/libwaei.la ../libgwaei/libgwaei.la
gwaei_CPPFLAGS = -I$(top_srcdir)/src/libwaei/include -I$(top_srcdir)/src/libgwaei/include -I$(top_srcdir)/src/gwaei/include $(GWAEI_CFLAGS) $(GWAEI_DEFS) $(DEFINITIONS)

if OS_CYGWIN
gwaei_CPPFLAGS +=-DOS_CYGWIN
endif
if OS_MINGW
gwaei_LDFLAGS =-Wl,-subsystem,windows 
MINGW_DEFS =-DCURL_STATICLIB -DOS_MINGW
gwaei_LDADD +=$(MINGW_LIBS) mingw/gwaei.res
gwaei_CPPFLAGS +=$(MINGW_CFLAGS) $(MINGW_DEFS)
endif

#pkgdata_DATA = mainwindow.ui
#EXTRA_DIST = mainwindow.ui



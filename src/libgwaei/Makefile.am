
SUBDIRS = include/libgwaei 

LIBRARY_VERSION = 0:0:0
ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" 

lib_LTLIBRARIES =libgwaei.la
libgwaei_la_SOURCES =libgwaei.c dictionarylistbox.c dictionarylistbox-callbacks.c dictionarylist.c dictionarylist-iter.c dictionarylist-callbacks.c searchwidget.c searchentry.c searchentry-callbacks.c resultsview.c resultstextview.c stackwidget.c texttagtable.c
libgwaei_la_LDFLAGS =-no-undefined -version-info $(LIBRARY_VERSION)  $(GWAEI_LIBS) 
libgwaei_la_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include -I$(top_srcdir)/src/libgwaei/include $(GWAEI_CFLAGS) $(DEFINITIONS) 
libgwaei_la_LIBADD =

if WITH_HUNSPELL
#libgwaei_la_LDFLAGS +=$(HUNSPELL_LIBS)
#libgwaei_la_CPPFLAGS +=$(HUNSPELL_CFLAGS) -DHUNSPELL_MYSPELL_DICTIONARY_PATH=\"$(HUNSPELL_MYSPELL_DICTIONARY_PATH)\"
#libgwaei_la_SOURCES += spellcheck.c spellcheck-callbacks.c
endif

if OS_MINGW
libgwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
MINGW_DEFS =-DCURL_STATICLIB 
libgwaei_la_LDFLAGS +=$(MINGW_LIBS)
libgwaei_la_CPPFLAGS +=$(MINGW_CFLAGS) $(MINGW_DEFS)
endif

pkgdata_DATA = searchwidget-menumodel-button.ui 
EXTRA_DIST = searchwidget-menumodel-button.ui


SUBDIRS = include/libgwaei 

LIBRARY_VERSION = 0:0:0
ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" 

lib_LTLIBRARIES =libgwaei.la
libgwaei_la_SOURCES =libgwaei.c actiongroup.c window.c window-callbacks.c  searchwidget.c searchwidget-callbacks.c searchentry.c searchentry-callbacks.c resultsview.c resultstextview.c resultstextview-callbacks.c texttagtable.c texttagtable-callbacks.c actionable.c menuable.c
libgwaei_la_SOURCES +=dictionarylistbox.c dictionarylistbox-callbacks.c dictionaryliststore.c dictionaryliststore-treemodel-interface.c dictionaryliststore-callbacks.c
libgwaei_la_SOURCES +=vocabularywordstore.c vocabularywordstore-callbacks.c vocabularywordstore-treemodel-interface.c vocabularywordview.c vocabularywordview-callbacks.c vocabularywordview-actionable-interface.c vocabularyliststore.c vocabularyliststore-treemodel-interface.c vocabularylistview.c vocabularylistview-callbacks.c vocabularylistview-actionable-interface.c vocabularywidget.c vocabularywidget-callbacks.c
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

pkgdata_DATA = searchwidget-menumodel-button.ui searchwidget-menumodel-window.ui vocabularywidget-menumodel-button.ui vocabularywidget-menumodel-window.ui
EXTRA_DIST = searchwidget-menumodel-button.ui searchwidget-menumodel-window.ui


SUBDIRS = include/libgwaei 

LIBRARY_VERSION = 0:0:0
ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" 

lib_LTLIBRARIES =libgwaei.la
libgwaei_la_SOURCES =libgwaei.c actiongroup.c actionable.c menuable.c resultsview.c 

libgwaei_la_SOURCES +=window.c window-callbacks.c

#SearchWidget component
libgwaei_la_SOURCES +=searchwidget.c searchwidget-callbacks.c searchwidget-actionable-interface.c searchwidget-menuable-interface.c
libgwaei_la_SOURCES +=texttagtable.c texttagtable-callbacks.c
libgwaei_la_SOURCES +=searchentry.c searchentry-callbacks.c searchentry-actionable-interface.c
libgwaei_la_SOURCES +=resultstextview.c resultstextview-callbacks.c resultstextview-actionable-interface.c resultstextview-resultsview-interface.c
libgwaei_la_SOURCES +=dictionarylistbox.c dictionarylistbox-callbacks.c
libgwaei_la_SOURCES +=dictionaryliststore.c dictionaryliststore-treemodel-interface.c dictionaryliststore-callbacks.c

#VocabularyWidget component
libgwaei_la_SOURCES +=vocabularywidget.c vocabularywidget-callbacks.c vocabularywidget-actionable-interface.c vocabularywidget-menuable-interface.c
libgwaei_la_SOURCES +=vocabularywordstore.c vocabularywordstore-callbacks.c vocabularywordstore-treemodel-interface.c vocabularywordstore-treedragsource-interface.c vocabularywordstore-treedragdest-interface.c vocabularywordstore-treesortable-interface.c
libgwaei_la_SOURCES +=vocabularywordview.c vocabularywordview-callbacks.c vocabularywordview-actionable-interface.c
libgwaei_la_SOURCES +=vocabularyliststore.c vocabularyliststore-callbacks.c vocabularyliststore-treemodel-interface.c vocabularyliststore-treedragsource-interface.c vocabularyliststore-treedragdest-interface.c vocabularyliststore-treesortable-interface.c 
libgwaei_la_SOURCES +=vocabularylistview.c vocabularylistview-callbacks.c vocabularylistview-actionable-interface.c
libgwaei_la_SOURCES +=addvocabularydialog.c addvocabularydialog-callbacks.c


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

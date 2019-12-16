
SUBDIRS = include/libwaei

LIBRARY_VERSION = 2:0:0
ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
dictionarylibdir = @dictionarylibdir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" -DPKGLIBDIR=\"$(pkglibdir)\" -DDICTIONARYLIBDIR=\"$(dictionarylibdir)\"

pkglib_LTLIBRARIES =libwaei.la
#libwaei_la_SOURCES =command.c
#libwaei_la_SOURCES +=application.c
libwaei_la_SOURCES =serializable.c mapped-file.c cache-file.c progress.c
libwaei_la_SOURCES +=iterable.c
#libwaei_la_SOURCES +=parenthesisnode.c querynode.c querynodematchmarker.c querynodematchinfo.c querynodecolumnmatchinfo.c
#libwaei_la_SOURCES +=dependancy.c condition.c filepath.c
#libwaei_la_SOURCES +=dictionarymodule.c
#libwaei_la_SOURCES +=dictionary.c dictionary-callbacks.c dictionarycache.c dictionarycachetree.c dictionarylist.c dictionarylist-callbacks.c
#libwaei_la_SOURCES +=dictionaryinstall.c dictionaryinstalllist.c dictionaryinstalllist-callbacks.c dictionaryinstallstatehistory.c dictionaryinstallstate.c
#libwaei_la_SOURCES +=result.c results.c
#libwaei_la_SOURCES +=utf8.c
#libwaei_la_SOURCES +=parsed.c parsedline.c
#libwaei_la_SOURCES +=indexed.c
#libwaei_la_SOURCES +=vocabulary.c word.c 
#libwaei_la_SOURCES +=range.c cachefile.c mappedfile.c io.c search.c preferences.c progress.c 
libwaei_la_LDFLAGS =-version-info $(LIBRARY_VERSION)  $(LIBWAEI_LIBS) -no-undefined -avoid-version
libwaei_la_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include $(LIBWAEI_CFLAGS) $(DEFINITIONS) 
libwaei_la_LIBADD =

#if WITH_MECAB
#libwaei_la_LIBADD +=$(MECAB_LIBS)
#libwaei_la_SOURCES +=morphologyengine-mecab.c
#endif

if WITH_HUNSPELL
libwaei_la_LIBADD +=$(HUNSPELL_LIBS)
libwaei_la_CPPFLAGS +=$(HUNSPELL_CFLAGS) -DHUNSPELL_MYSPELL_DICTIONARY_PATH=\"$(HUNSPELL_MYSPELL_DICTIONARY_PATH)\"
#libwaei_la_SOURCES +=morphologyengine-hunspell.c
endif

if OS_MINGW
libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
MINGW_DEFS =-DCURL_STATICLIB 
libwaei_la_LDFLAGS +=$(MINGW_LIBS)
libwaei_la_CPPFLAGS +=$(MINGW_CFLAGS) $(MINGW_DEFS)
endif


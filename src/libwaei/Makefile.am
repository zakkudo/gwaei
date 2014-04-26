
SUBDIRS = include/libwaei 

LIBRARY_VERSION = 2:0:0
ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" 

lib_LTLIBRARIES =libwaei.la
libwaei_la_SOURCES =libwaei.c dictionary.c dictionary-index.c dictionary-regex.c dictionarydata.c dictionaryinstall.c dictionary-callbacks.c edictionary.c kanjidictionary.c exampledictionary.c index.c unknowndictionary.c dictionarylist.c dictionarylist-callbacks.c range.c utilities.c io.c regex.c search.c result.c preferences.c vocabulary.c word.c morphologystring.c morphologyengine.c progress.c 
libwaei_la_LDFLAGS =-no-undefined -version-info $(LIBRARY_VERSION)  $(LIBWAEI_LIBS)
libwaei_la_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include $(LIBWAEI_CFLAGS) $(DEFINITIONS) 
libwaei_la_LIBADD =

if WITH_MECAB
libwaei_la_LIBADD +=$(MECAB_LIBS)
#libwaei_la_SOURCES +=morphologyengine-mecab.c
endif

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


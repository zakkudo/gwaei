
SUBDIRS = include/libwaei 

LIBRARY_VERSION = 2:0:0
ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" 

lib_LTLIBRARIES =libwaei.la
libwaei_la_SOURCES =libwaei.c
libwaei_la_SOURCES +=dependancy.c condition.c filepath.c
libwaei_la_SOURCES +=dictionary.c dictionary-callbacks.c dictionarybuffer.c dictionarylist.c dictionarylist-callbacks.c
libwaei_la_SOURCES +=dictionaryinstall.c dictionaryinstalllist.c dictionaryinstalllist-callbacks.c dictionaryinstallstatehistory.c dictionaryinstallstate.c
libwaei_la_SOURCES +=edictionary.c kanjidictionary.c exampledictionary.c radicalsdictionary.c unknowndictionary.c
libwaei_la_SOURCES +=utilities.c utf8.c
libwaei_la_SOURCES +=line.c
libwaei_la_SOURCES +=morphologystring.c morphologyengine.c
libwaei_la_SOURCES +=vocabulary.c word.c 
libwaei_la_SOURCES +=range.c cachefile.c io.c regex.c search.c preferences.c progress.c 
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


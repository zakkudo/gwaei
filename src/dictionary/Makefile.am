
SUBDIRS = include/libwaei/dictionary

ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
dictionarylibdir = @dictionarylibdir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" -DPKGLIBDIR=\"$(pkglibdir)\" -DDICTIONARYLIBDIR=\"$(dictionarylibdir)\"

ALL_LDFLAGS=-no-undefined -module -shared -avoid-version -export-dynamic 
ALL_CPPFLAGS=-I$(top_srcdir)/src/libwaei/include -I$(top_srcdir)/src/dictionary/include $(LIBWAEI_CFLAGS) $(LIBWAEI_DEFS) $(DEFINITIONS)
ALL_LIBADD=$(LIBWAEI_LIBS) ../libwaei/libwaei.la

dictionarylib_LTLIBRARIES =libedictionary.la libkanjidictionary.la libradicalsdictionary.la libexampledictionary.la libunknowndictionary.la

libedictionary_la_SOURCES =edictionary.c
libedictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libedictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libedictionary_la_LIBADD =$(ALL_LIBADD)

libkanjidictionary_la_SOURCES =kanjidictionary.c
libkanjidictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libkanjidictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libkanjidictionary_la_LIBADD =$(ALL_LIBADD)

libradicalsdictionary_la_SOURCES =radicalsdictionary.c
libradicalsdictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libradicalsdictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libradicalsdictionary_la_LIBADD =$(ALL_LIBADD)

libexampledictionary_la_SOURCES =exampledictionary.c
libexampledictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libexampledictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libexampledictionary_la_LIBADD =$(ALL_LIBADD)

libunknowndictionary_la_SOURCES =unknowndictionary.c
libunknowndictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libunknowndictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libunknowndictionary_la_LIBADD =$(ALL_LIBADD)

if OS_MINGW
ALL_LDFLAGS +=-Wl,-subsystem,windows 
ALL_LDFLAGS +=$(MINGW_LIBS)
ALL_CPPFLAGS +=$(MINGW_CFLAGS) $(MINGW_DEFS)
endif


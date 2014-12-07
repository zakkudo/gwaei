
SUBDIRS = include/libwaei 

LIBRARY_VERSION = 1:0:0
ACLOCAL_AMFLAGS = -I m4
datadir = @datadir@
DEFINITIONS =-DDATADIR2=\"$(datadir)\" -DGWAEI_LOCALEDIR=\"$(GWAEI_LOCALEDIR)\" 

ALL_LDFLAGS=-no-undefined -version-info $(LIBRARY_VERSION)  $(LIBWAEI_LIBS)
ALL_CPPFLAGS=-I$(top_srcdir)/src/libwaei/include -I$(top_srcdir)/src/libwaei-dictionary/include $(WAEI_CFLAGS) $(WAEI_DEFS) $(DEFINITIONS)
ALL_LDADD=$(WAEI_LIBS) ../libwaei/libwaei.la

lib_LTLIBRARIES =libwaei-edictionary.la libwaei-kanjidictionary.la libwaei-radicalsdictionary.la libwaei-exampledictionary.la libwaei-unknowndictionary.la

libwaei_edictionary_la_SOURCES =edictionary.c
libwaei_edictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libwaei_edictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libwaei_edictionary_la_LDADD =$(ALL_LDADD)

libwaei_kanjidictionary_la_SOURCES =kanjidictionary.c
libwaei_kanjidictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libwaei_kanjidictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libwaei_kanjidictionary_la_LDADD =$(ALL_LDADD)

libwaei_radicalsdictionary_la_SOURCES =radicalsdictionary.c
libwaei_radicalsdictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libwaei_radicalsdictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libwaei_radicaldictionary_la_LDADD =$(ALL_LDADD)

libwaei_exampledictionary_la_SOURCES =exampledictionary.c
libwaei_exampledictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libwaei_exampledictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libwaei_exampledictionary_la_LDADD =$(ALL_LDADD)

libwaei_unknown_la_SOURCES =unknowndictionary.c
libwaei_unknowndictionary_la_LDFLAGS =$(ALL_LDFLAGS)
libwaei_unknowndictionary_la_CPPFLAGS =$(ALL_CPPFLAGS)
libwaei_unknowndictionary_la_LDADD =$(ALL_LDADD)

if OS_MINGW
libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
libwaei_la_LDFLAGS +=$(MINGW_LIBS)
libwaei_la_CPPFLAGS +=$(MINGW_CFLAGS) $(MINGW_DEFS)
endif


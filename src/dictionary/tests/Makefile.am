
GTESTER = gtester -k
SUBDIRS = 

ALL_LDFLAGS=-no-undefined
ALL_LDADD=$(LIBWAEI_LIBS) $(top_srcdir)/src/libwaei/libwaei.la
ALL_CPPFLAGS=-I$(top_srcdir)/src/libwaei/include -I$(top_srcdir)/src/dictionary/include $(LIBWAEI_CFLAGS) $(LIBWAEI_DEFS) $(DEFINITIONS) -DDICTIONARYLIBDIR=\"$(dictionarylibdir)\" -DTESTDATADIR=\"$(top_srcdir)/src/dictionary/tests/data/\"


noinst_PROGRAMS =$(TEST_PROGS)

TEST_PROGS =testdictionarymodule testedictionary testkanjidictionary testradicalsdictionary testexampledictionary testunknowndictionary


testdictionarymodule_SOURCES =testdictionarymodule.c
testdictionarymodule_LDFLAGS =$(ALL_LDFLAGS)
testdictionarymodule_CPPFLAGS =$(ALL_CPPFLAGS)
testdictionarymodule_LDADD =$(ALL_LDADD)

testedictionary_SOURCES =testedictionary.c
testedictionary_LDFLAGS  =$(ALL_LDFLAGS)
testedictionary_CPPFLAGS =$(ALL_CPPFLAGS)
testedictionary_LDADD =$(ALL_LDADD)

testkanjidictionary_SOURCES =testkanjidictionary.c
testkanjidictionary_LDFLAGS  =$(ALL_LDFLAGS)
testkanjidictionary_CPPFLAGS =$(ALL_CPPFLAGS)
testkanjidictionary_LDADD =$(ALL_LDADD)

testradicalsdictionary_SOURCES =testradicalsdictionary.c
testradicalsdictionary_LDFLAGS  =$(ALL_LDFLAGS)
testradicalsdictionary_CPPFLAGS =$(ALL_CPPFLAGS)
testradicalsdictionary_LDADD =$(ALL_LDADD)

testexampledictionary_SOURCES =testexampledictionary.c
testexampledictionary_LDFLAGS  =$(ALL_LDFLAGS)
testexampledictionary_CPPFLAGS =$(ALL_CPPFLAGS)
testexampledictionary_LDADD =$(ALL_LDADD)

testunknowndictionary_SOURCES =testunknowndictionary.c
testunknowndictionary_LDFLAGS  =$(ALL_LDFLAGS)
testunknowndictionary_CPPFLAGS =$(ALL_CPPFLAGS)
testunknowndictionary_LDADD =$(ALL_LDADD)

if !OS_MINGW
#libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
endif

test:
	{ set -e && ${GTESTER} --verbose ${TEST_PROGS}; }

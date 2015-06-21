
GTESTER = gtester -k
SUBDIRS = 

ALL_LDFLAGS=-no-undefined
ALL_LDADD=$(LIBWAEI_LIBS) $(top_srcdir)/src/libwaei/libwaei.la
ALL_CPPFLAGS=-I$(top_srcdir)/src/libwaei/include -I$(top_srcdir)/src/dictionary/include $(LIBWAEI_CFLAGS) $(LIBWAEI_DEFS) $(DEFINITIONS) -DDICTIONARYLIBDIR=\"$(dictionarylibdir)\" -DTESTDATADIR=\"$(top_srcdir)/dictionary/tests/data/\"


noinst_PROGRAMS =$(TEST_PROGS)

TEST_PROGS =testdictionarymodule testedictionary

testdictionarymodule_SOURCES =testdictionarymodule.c
testdictionarymodule_LDFLAGS =$(ALL_LDFLAGS)
testdictionarymodule_CPPFLAGS =$(ALL_CPPFLAGS)
testdictionarymodule_LDADD =$(ALL_LDADD)

testedictionary_SOURCES =testedictionary.c
testedictionary_LDFLAGS  =$(ALL_LDFLAGS)
testedictionary_CPPFLAGS =$(ALL_CPPFLAGS)
testedictionary_LDADD =$(ALL_LDADD)

if !OS_MINGW
#libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
endif

test:
	{ set -e && ${GTESTER} --verbose ${TEST_PROGS}; }


GTESTER = gtester -k
SUBDIRS = 

AM_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include -I$(top_srcdir)/src/dictionary/include $(LIBWAEI_CFLAGS) -DDICTIONARYLIBDIR=\"$(dictionarylibdir)\"


noinst_PROGRAMS =$(TEST_PROGS)

TEST_PROGS =testdictionarymodule testedictionary

testdictionarymodule_SOURCES =testdictionarymodule.c
testdictionarymodule_LDADD   =../../libwaei/libwaei.la

testedictionary_SOURCES =testedictionary.c
testedictionary_LDADD   =../../libwaei/libwaei.la

if !OS_MINGW
#libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
endif

test:
	{ set -e && ${GTESTER} --verbose ${TEST_PROGS}; }

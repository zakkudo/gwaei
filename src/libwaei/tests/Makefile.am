
GTESTER = gtester -k
SUBDIRS = 

AM_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include $(WAEI_CFLAGS)

noinst_PROGRAMS =$(TEST_PROGS)

TEST_PROGS   = index morphology edictionary

morphology_SOURCES =morphology.c
morphology_LDADD   =$(WAEI_LIBS) ../libwaei.la

index_SOURCES   =index.c 
index_LDADD     =$(WAEI_LIBS) ../libwaei.la

edictionary_SOURCES   =edictionary.c 
edictionary_LDADD     =$(WAEI_LIBS) ../libwaei.la

if !OS_MINGW
#libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
endif

test:
	{ set -e && ${GTESTER} --verbose ${TEST_PROGS}; }

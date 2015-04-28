
GTESTER = gtester -k
SUBDIRS = 

AM_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include $(WAEI_CFLAGS)

noinst_PROGRAMS =$(TEST_PROGS)

TEST_PROGS   = testparenthesisnode testquerynode testutf8 testprogress

testparenthesisnode_SOURCES =testparenthesisnode.c
testparenthesisnode_LDADD   =../libwaei.la

testquerynode_SOURCES =testquerynode.c
testquerynode_LDADD   =../libwaei.la

testutf8_SOURCES =testutf8.c
testutf8_LDADD   =../libwaei.la

testprogress_SOURCES =testprogress.c
testprogress_LDADD   =../libwaei.la

if !OS_MINGW
#libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
endif

test:
	{ set -e && ${GTESTER} --verbose ${TEST_PROGS}; }

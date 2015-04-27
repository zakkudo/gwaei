
GTESTER = gtester -k
SUBDIRS = 

AM_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include $(WAEI_CFLAGS)

noinst_PROGRAMS =$(TEST_PROGS)

TEST_PROGS   = parenthesisnode querynode utf8

parenthesisnode_SOURCES =parenthesisnode.c
parenthesisnode_LDADD   =../libwaei.la

querynode_SOURCES =querynode.c
querynode_LDADD   =../libwaei.la

utf8_SOURCES =utf8.c
utf8_LDADD   =../libwaei.la

if !OS_MINGW
#libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
endif

test:
	{ set -e && ${GTESTER} --verbose ${TEST_PROGS}; }

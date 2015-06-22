
GTESTER = gtester -k
SUBDIRS = 

AM_CPPFLAGS =-I$(top_srcdir)/src/libwaei/include $(WAEI_CFLAGS)

noinst_PROGRAMS =$(TEST_PROGS)

TEST_PROGS   = testparenthesisnode testquerynode testutf8 testprogress testparsedline testquerynodematchmarker testquerynodecolumnmatchinfo testquerynodematchinfo testparsed testmappedfile testcachefile testio testdictionary testdictionarycache

testdictionarycache_SOURCES =testdictionarycache.c
testdictionarycache_LDADD   =../libwaei.la

testparenthesisnode_SOURCES =testparenthesisnode.c
testparenthesisnode_LDADD   =../libwaei.la

testquerynode_SOURCES =testquerynode.c
testquerynode_LDADD   =../libwaei.la

testutf8_SOURCES =testutf8.c
testutf8_LDADD   =../libwaei.la

testprogress_SOURCES =testprogress.c
testprogress_LDADD   =../libwaei.la

testparsedline_SOURCES =testparsedline.c
testparsedline_LDADD   =../libwaei.la

testparsed_SOURCES =testparsed.c
testparsed_LDADD   =../libwaei.la

testdictionary_SOURCES =testdictionary.c
testdictionary_LDADD   =../libwaei.la

testquerynodematchmarker_SOURCES =testquerynodematchmarker.c
testquerynodematchmarker_LDADD   =../libwaei.la

testquerynodecolumnmatchinfo_SOURCES =testquerynodecolumnmatchinfo.c
testquerynodecolumnmatchinfo_LDADD   =../libwaei.la

testquerynodematchinfo_SOURCES =testquerynodematchinfo.c
testquerynodematchinfo_LDADD   =../libwaei.la

testmappedfile_SOURCES =testmappedfile.c
testmappedfile_LDADD   =../libwaei.la

testcachefile_SOURCES =testcachefile.c
testcachefile_LDADD   =../libwaei.la

testio_SOURCES =testio.c
testio_LDADD   =../libwaei.la

if !OS_MINGW
#libwaei_la_LDFLAGS +=-Wl,-subsystem,windows 
endif

test:
	{ set -e && ${GTESTER} --verbose ${TEST_PROGS}; }

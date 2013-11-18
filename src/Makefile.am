## Process this file with automake to produce Makefile.in

SUBDIRS = libwaei schemas 
#SUBDIRS += libwaei/tests

if !OS_MINGW
SUBDIRS += waei
endif

if WITH_GNOME
SUBDIRS += libgwaei gwaei kpengine desktop images
endif


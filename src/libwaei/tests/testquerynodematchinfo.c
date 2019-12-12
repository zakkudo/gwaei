#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/querynodematchinfo.h>

struct _Fixture {
  LwQueryNodeMatchInfo * match_info;
  LwQueryNodeColumnMatchInfo * word;
  LwQueryNodeColumnMatchInfo * readings;
  LwQueryNodeColumnMatchInfo * definitions;
};
typedef struct _Fixture Fixture;


static gchar const * CONTENTS = "語彙\0ごい\0word\0vocabulary\0vocab";
static gchar const * WORD[2] = {0};
static gchar const * READINGS[2] = {0};
static gchar const * DEFINITIONS[4] = {0};


void setup (Fixture *fixture, gconstpointer data)
{
    fixture->match_info = lw_querynodematchinfo_new ();

    WORD[0] = CONTENTS;
    WORD[1] = NULL;
    fixture->word = lw_querynodecolumnmatchinfo_new (1, WORD);

    printf("SETUP\n");
    lw_querynodematchinfo_set_column (fixture->match_info, fixture->word);
    lw_querynodecolumnmatchinfo_unref (fixture->word);

    READINGS[0] = CONTENTS + strlen("語彙 ");
    READINGS[1] = NULL;
    fixture->readings = lw_querynodecolumnmatchinfo_new (2, READINGS);
    lw_querynodematchinfo_set_column (fixture->match_info, fixture->readings);
    lw_querynodecolumnmatchinfo_unref (fixture->readings);

    DEFINITIONS[0] = CONTENTS + strlen("語彙 ごい ");
    DEFINITIONS[1] = CONTENTS + strlen("語彙 ごい word ");
    DEFINITIONS[2] = CONTENTS + strlen("語彙 ごい word vocabulary ");
    DEFINITIONS[3] = NULL;
    fixture->definitions = lw_querynodecolumnmatchinfo_new (3, DEFINITIONS);
    lw_querynodematchinfo_set_column (fixture->match_info, fixture->definitions);
    lw_querynodecolumnmatchinfo_unref (fixture->definitions);
}


void teardown (Fixture *fixture, gconstpointer data)
{
    lw_querynodematchinfo_unref (fixture->match_info);
}


void
get_column_that_is_unset (Fixture       * fixture,
                          gconstpointer   data)
{
    LwQueryNodeColumnMatchInfo * column = NULL;
    column = lw_querynodematchinfo_get_column (fixture->match_info, 100);
    g_assert(column == NULL);
}


void
get_column_that_is_set (Fixture       * fixture,
                        gconstpointer   data)
{
    LwQueryNodeColumnMatchInfo * column = NULL;
    column = lw_querynodematchinfo_get_column (fixture->match_info, 1);
    printf("get_column %d %d\n", column, fixture->word);
    g_assert(column == fixture->word);
}


void
set_column_twice (Fixture       * fixture,
                  gconstpointer   data)
{
    gint column = lw_querynodecolumnmatchinfo_get_column (fixture->word);
    lw_querynodematchinfo_set_column (fixture->match_info, fixture->word);
    g_assert (lw_querynodematchinfo_get_column (fixture->match_info, column) == fixture->word);
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/get_column/that_is_set", Fixture, NULL, setup, get_column_that_is_set, teardown);
    g_test_add ("/get_column/that_is_unset", Fixture, NULL, setup, get_column_that_is_unset, teardown);

    g_test_add ("/set_column/twice", Fixture, NULL, setup, set_column_twice, teardown);

    return g_test_run();
}




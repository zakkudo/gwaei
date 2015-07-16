#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/results.h>

#include "testdictionary.h"


struct _Fixture {LwResults * results; LwDictionaryCache * dictionary_cache; LwParsedLine line;};
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->dictionary_cache = lw_dictionarycache_new ("test", LW_TYPE_TESTDICTIONARY, 0);
    fixture->results = lw_results_new (fixture->dictionary_cache);
}

void teardown (Fixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->results);
    g_object_unref (fixture->dictionary_cache);
}


void
new (Fixture *fixture, gconstpointer data)
{
  g_assert (fixture->dictionary_cache == lw_results_get_dictionarycache (fixture->results));
  g_assert_cmpint (0, ==, g_sequence_get_length (lw_results_get_sequence (fixture->results)));
}


void
append_line (Fixture *fixture, gconstpointer data)
{
    lw_results_append_line (fixture->results, &fixture->line);
    g_assert (fixture->dictionary_cache == lw_results_get_dictionarycache (fixture->results));
    g_assert_cmpint (1, ==, g_sequence_get_length (lw_results_get_sequence (fixture->results)));
}





gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/new", Fixture, NULL, setup, new, teardown);
    g_test_add ("/append_line", Fixture, NULL, setup, append_line, teardown);

    return g_test_run ();
}


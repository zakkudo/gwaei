#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/results.h>

#include "testdictionary.h"


struct _Fixture {LwResults * results; LwDictionaryCache * dictionary_cache;};
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
empty (Fixture *fixture, gconstpointer data)
{
  /*TODO
    //Declarations
    LwResults * results = NULL;
    GError * error = NULL;

    results = lw_results_new ();

    lw_results_dictionary (results, FALSE, &error);

    g_object_unref (results);
    */
}





gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/empty", Fixture, NULL, setup, empty, teardown);

    return g_test_run ();
}


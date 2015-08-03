#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/utf8.h>
#include <libwaei/search.h>

#include "testdictionary.h"


struct _Fixture { LwProgress * progress; GArray *steps; LwDictionary * dictionary; DictionaryConfig * config;};
typedef struct _Fixture Fixture;

static void
append_progress (LwProgress * self, Fixture * fixture)
{
    gint percent = (gint) (lw_progress_get_fraction (self) * 100.0);
    g_array_append_val (fixture->steps, percent);
}


static void
cancel_progress (LwProgress *self, Fixture * fixture)
{
    if (lw_progress_get_fraction (self) > .5)
    {
      lw_progress_cancel (self);
    }
}


void setup (Fixture *fixture, gconstpointer data)
{
    fixture->config = dictionary_config_new ();

    fixture->progress = lw_progress_new ();
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (append_progress), fixture);
    lw_progress_set_prefered_chunk_size (fixture->progress, 2);
    fixture->steps = g_array_sized_new (FALSE, TRUE, sizeof(gint), 20);

    GType type = g_type_from_name ("LwEDictionary");
    fixture->dictionary = lw_dictionary_new (type, TESTDICTIONARYDATADIR G_DIR_SEPARATOR_S "edictionary.data");
}

void teardown (Fixture *fixture, gconstpointer data)
{
    dictionary_config_free (fixture->config);

    g_object_unref (fixture->progress);
    fixture->progress = NULL;
    g_array_free (fixture->steps, TRUE);
    fixture->steps = NULL;

    g_object_unref (fixture->dictionary);
}


void
dictionary (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwSearch * search = NULL;
    LwResults * results = NULL;

    //Initializations
    search = lw_search_new ("decimal", fixture->dictionary, 0);
    results = lw_search_query_results (search);

    g_object_unref (search);
    g_object_unref (results);
}





gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/dictionary", Fixture, NULL, setup, dictionary, teardown);

    return g_test_run ();
}

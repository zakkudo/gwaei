#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>


struct _DictionaryFixture {
  LwMorphologyEngine *engine;
  LwDictionary *dictionary;
};
typedef struct _DictionaryFixture DictionaryFixture;


void
dictionary_test_setup (DictionaryFixture *fixture, 
                       gconstpointer      data)
{
    g_setenv ("LIBWAEI_DATA_FOLDER", "data", TRUE);
    memset(fixture, 0, sizeof(DictionaryFixture));
    fixture->engine = lw_morphologyengine_new ("en_US");
    fixture->dictionary = lw_edictionary_new ("English", fixture->engine);
    lw_dictionary_index (fixture->dictionary, NULL, NULL);
}


void
dictionary_test_teardown (DictionaryFixture *fixture, 
                          gconstpointer      data)
{
    g_object_unref (fixture->engine); fixture->engine = NULL;
    g_object_unref (fixture->dictionary); fixture->dictionary = NULL;
    g_remove ("data/index/e/English");
}


void
dictionary_test_english_stem (DictionaryFixture *fixture, 
                              gconstpointer      data)
{
/* TODO
    LwMorphologyList *query = lw_morphologyengine_analyze (fixture->engine, "faithfulness", FALSE);
    GHashTable* table = lw_dictionary_index_search (dictionary, query, 0);

    g_assert (g_list_length (resultlist) == 1);
*/
}


void
dictionary_test_english_and (DictionaryFixture *fixture, 
                             gconstpointer      data)
{
/*TODO
    LwMorphologyList *query = lw_morphologyengine_analyze (fixture->engine, "in the", FALSE);
    GList *resultlist = lw_dictionary_get_indexed_results (fixture->dictionary, query);
    g_assert (g_list_length(resultlist) == 1);
*/
}

void
dictionary_test_english_repeat_words (DictionaryFixture *fixture, 
                                      gconstpointer      data)
{
/*TODO
    LwMorphologyList *query = lw_morphologyengine_analyze (fixture->engine, "hand-to-hand", FALSE);
    GList *resultlist = lw_dictionary_get_indexed_results (fixture->dictionary, query);
    g_assert (g_list_length(resultlist) == 1);

    LwMorphologyList *query = lw_morphologyengine_analyze (fixture->engine, "monkey monkey", FALSE);
    GList *resultlist = lw_dictionary_get_indexed_results (fixture->dictionary, query);
    g_assert (g_list_length(resultlist) == 1);
*/
}


gint
main (gint argc, gchar *argv[])
{
/*TODO
    g_type_init ();
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/libwaei/edictionary/english/stem", DictionaryFixture, NULL, dictionary_test_setup, dictionary_test_english_stem, dictionary_test_teardown);
    g_test_add ("/libwaei/edictionary/english/and", DictionaryFixture, NULL, dictionary_test_setup, dictionary_test_english_and, dictionary_test_teardown);

  return g_test_run();
*/ return 0;
}



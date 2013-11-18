#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

/*
  Methods tested

  lw_index_append_data_offset
  lw_index_get_data_offsets
  lw_index_get_data_offsets_length
  lw_index_search
  lw_index_data_is_valid
*/

struct _IndexFixture {
  LwIndex *index;
  LwDictionaryData *data;
  LwMorphologyEngine *engine;
};
typedef struct _IndexFixture IndexFixture;


void
index_test_setup (IndexFixture *fixture, gconstpointer data)
{
    memset(fixture, 0, sizeof(IndexFixture));
    fixture->engine = lw_morphologyengine_new ("en_US");
}


void
index_test_teardown (IndexFixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->engine); fixture->engine = NULL;
    g_remove ("data/index/e/English");
}


void
index_offset_read_write_test (IndexFixture *fixture, gconstpointer data)
{
/*TODO
    //Arrange
    fixture->index = lw_index_new (fixture->engine);

    gint expected_offsets[] = { 10, 20, 35 };
    gint expected_length = G_N_ELEMENTS (expected_offsets);
    gint i = 0;
    const gchar *KEY = "test";

    for (i = 0; i < expected_length; i++)
    {
      lw_index_append_data_offset (fixture->index, KEY, expected_offsets[i]);
    }

    //Act
    LwOffsets* actual_offsets = lw_index_get_data_offsets (fixture->index, KEY);
    gint actual_length = lw_index_get_data_offsets_length (fixture->index, KEY);

    //Assert
    g_assert_cmpuint (expected_length, ==, actual_length);

    for (i = 0; i < expected_length; i++)
    {
      g_assert_cmpuint (expected_offsets[i], ==, actual_offsets[i]);
    }

    lw_index_free (fixture->index);
*/
}


void
index_parse_string_test (IndexFixture *fixture, gconstpointer data)
{
/*TODO
    //Arrange
    fixture->index = lw_index_new (fixture->engine);

    gchar *text[] = {
      "This is an english Sentence.",
      "これが日本語の文です。行って着ます。",
      "This is an english and 日本語のミックス。",
      NULL
    };
    gchar **iter;

    for (iter = text; *iter != NULL; iter++)
    {
      lw_index_add_string (fixture->index, *iter, *iter - *text);
    }

    //LwOffsets* actual_offsets = lw_index_get_data_offsets (fixture->index, "this");
    gint actual_length = lw_index_get_data_offsets_length (fixture->index, "This");

    g_assert_cmpuint (2, ==, actual_length);

    lw_index_free (fixture->index);
*/
}


void
index_index_file_test (IndexFixture *fixture, gconstpointer data)
{
    const gchar* PATH = "data/dictionaries/e/English";

    fixture->data = lw_dictionarydata_new ();
    lw_dictionarydata_create (fixture->data, PATH);

    fixture->index = lw_index_new (fixture->engine); 
    lw_index_create (fixture->index, fixture->data, NULL, NULL);

    //Verity checksum
    g_assert (strcmp(fixture->index->checksum, fixture->data->checksum) == 0);  

    LwMorphologyList *query = lw_morphologyengine_analyze (fixture->engine, "decimal", FALSE);
    GList *results = lw_index_get_matches_for_morphologylist (fixture->index, LW_INDEX_TABLE_RAW, query);

    {
      GList *link;
      const gchar *EXPECTED_STRING = "１０進 [じゅっしん] /(adj-na,adj-no) decimal/denary/deciam/";
      for (link = results; link != NULL; link = link->next)
      {
        const gchar *ACTUAL_STRING = lw_dictionarydata_get_string (fixture->data, GPOINTER_TO_UINT (link->data));
        g_assert (g_strcmp0 (ACTUAL_STRING, EXPECTED_STRING) == 0);
      }
    }

    lw_morphologylist_free (query);
    lw_index_free (fixture->index); fixture->index = NULL;
    lw_dictionarydata_free (fixture->data); fixture->data = NULL;
}


void
index_load_save_test (IndexFixture *fixture, gconstpointer data)
{
/* TODO
    fixture->index = NULL;

    const gchar* PATH = "data/index/e/English";
    g_mkdir_with_parents ("data/index/e", 0755);

    LwIndex *saveindex = lw_index_new (fixture->engine);
    saveindex->table = g_hash_table_new (g_direct_hash, g_direct_equal);
    saveindex->buffer = g_strdup ("checksum");
    saveindex->checksum = saveindex->buffer;
    lw_index_append_data_offset (saveindex, "fish", 1);
    lw_index_append_data_offset (saveindex, "日本語", 2);
    lw_index_append_data_offset (saveindex, "ばか", 3);
    lw_index_append_data_offset (saveindex, "strike", 4);
    lw_index_append_data_offset (saveindex, "pork", 5);
    lw_index_save (saveindex, PATH, NULL, NULL);

    LwIndex *loadindex = lw_index_new (fixture->engine);
    lw_index_load (loadindex, PATH, NULL, NULL);

    lw_index_are_equal (saveindex, loadindex);
     
    lw_index_free (loadindex); loadindex = NULL;
    lw_index_free (saveindex); saveindex = NULL;
*/
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    lw_regex_initialize ();

    g_test_add ("/libwaei/index/read_write_offsets", IndexFixture, NULL, index_test_setup, index_offset_read_write_test, index_test_teardown);
    g_test_add ("/libwaei/index/parse_string", IndexFixture, NULL, index_test_setup, index_parse_string_test, index_test_teardown);
    g_test_add ("/libwaei/index/index_file", IndexFixture, NULL, index_test_setup, index_index_file_test, index_test_teardown);
    g_test_add ("/libwaei/index/load_save", IndexFixture, NULL, index_test_setup, index_load_save_test, index_test_teardown);

    return g_test_run();
}



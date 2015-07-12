#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarymodule.h>
#include <libwaei/dictionary/radicalsdictionary.h>


static GTypeModule * module = NULL;

struct _Fixture {
  LwDictionary * dictionary;
  gchar const * FILENAME;
  gchar * cachetmpl;
  gchar * cachedir;
  gchar * datatmpl;
  gchar * datadir;
};
typedef struct _Fixture Fixture;


void setup (Fixture *fixture, gconstpointer data)
{
  fixture->cachetmpl = g_build_filename (g_get_tmp_dir (), "testcachedir-XXXXXX", NULL);
  fixture->cachedir = g_mkdtemp_full (fixture->cachetmpl, 0700);
  g_setenv("DICTIONARYCACHEDIR", fixture->cachedir, TRUE);

  fixture->datatmpl = g_build_filename (g_get_tmp_dir (), "testdatadir-XXXXXX", NULL);
  fixture->datadir = g_mkdtemp_full (fixture->datatmpl, 0700);
  g_setenv("DICTIONARYDATADIR", fixture->datadir, TRUE);

  if (module == NULL)
  {
    module = lw_dictionarymodule_new (".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libradicalsdictionary." G_MODULE_SUFFIX);
  }

  GType type = g_type_from_name ("LwRadicalsDictionary");
  fixture->FILENAME = TESTDATADIR "radicalsdictionary.data";
  fixture->dictionary = lw_dictionary_new (type, fixture->FILENAME);
}


void teardown (Fixture *fixture, gconstpointer data)
{
  if (fixture->dictionary) g_object_unref (fixture->dictionary);
  g_remove (fixture->cachedir);
  g_free (fixture->cachedir);

  g_remove (fixture->datadir);
  g_free(fixture->datadir);
}


void
get_contents (Fixture * fixture, gconstpointer data)
{
  gchar * contents = NULL;

  g_file_get_contents (fixture->FILENAME, &contents, NULL, NULL);

  g_assert_cmpstr (contents, ==, lw_dictionary_get_contents (fixture->dictionary));
  g_assert_nonnull (contents);

  g_free (contents);
}


void
get_contents_length (Fixture * fixture, gconstpointer data)
{
  gsize length = 0;
  gchar * contents = NULL;

  g_file_get_contents (fixture->FILENAME, &contents, &length, NULL);

  g_assert_cmpuint (length, ==, lw_dictionary_contents_length (fixture->dictionary));

  g_free (contents);
}


void
get_contents_checksum (Fixture * fixture, gconstpointer data)
{

  gsize length = 0;
  gchar * contents = NULL;
  gchar * checksum = NULL;

  g_file_get_contents (fixture->FILENAME, &contents, &length, NULL);
  checksum = g_compute_checksum_for_data (LW_DICTIONARY_CHECKSUM, contents, length);

  g_assert_cmpstr (checksum, ==, lw_dictionary_get_contents_checksum (fixture->dictionary));

  g_free (contents);
}


void
get_contents_path (Fixture * fixture, gconstpointer data)
{
  gsize length = 0;
  gchar * contents = NULL;

  g_assert_cmpstr (TESTDATADIR "radicalsdictionary.data", ==, lw_dictionary_get_contents_path (fixture->dictionary));

  g_free (contents);
}


void
get_name_when_unset (Fixture * fixture, gconstpointer data)
{
  g_assert_cmpstr ("radicalsdictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
}

void
get_id (Fixture * fixture, gconstpointer data)
{
  g_assert_cmpstr ("LwRadicalsDictionary/radicalsdictionary.data", ==, lw_dictionary_get_id (fixture->dictionary));
}

void
set_name_to_null (Fixture * fixture, gconstpointer data)
{
  lw_dictionary_set_name (fixture->dictionary, NULL);

  g_assert_cmpstr (NULL, ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr ("radicalsdictionary.data", ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, <, lw_dictionary_contents_length (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents_checksum (fixture->dictionary));
  g_assert_cmpstr ("LwRadicalsDictionary/radicalsdictionary.data", ==, lw_dictionary_get_id (fixture->dictionary));
}


void
set_name_to_something_else (Fixture * fixture, gconstpointer data)
{
  lw_dictionary_set_name (fixture->dictionary, "new test name");

  g_assert_cmpstr ("new test name", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr ("radicalsdictionary.data", ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, <, lw_dictionary_contents_length (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents_checksum (fixture->dictionary));
  g_assert_cmpstr ("LwRadicalsDictionary/radicalsdictionary.data", ==, lw_dictionary_get_id (fixture->dictionary));
}


void
set_contents_filename_to_null (Fixture * fixture, gconstpointer data)
{
  lw_dictionary_set_contents_filename (fixture->dictionary, NULL);

  g_assert_cmpstr ("radicalsdictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents_path (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, ==, lw_dictionary_contents_length (fixture->dictionary));
  g_assert_null (lw_dictionary_get_contents_checksum (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_id (fixture->dictionary));
}


void
set_contents_filename_to_something_else (Fixture * fixture, gconstpointer data)
{
  gchar * contents_path = g_build_filename (fixture->datadir, "libwaei", "dictionary", "LwRadicalsDictionary", "test new filename", NULL);

  lw_dictionary_set_contents_filename (fixture->dictionary, "test new filename");

  g_assert_cmpstr ("radicalsdictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr ("test new filename", ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_cmpstr (contents_path, ==, lw_dictionary_get_contents_path (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, ==, lw_dictionary_contents_length (fixture->dictionary));
  g_assert_null (lw_dictionary_get_contents_checksum (fixture->dictionary));
  g_assert_cmpstr ("LwRadicalsDictionary/test new filename", ==, lw_dictionary_get_id (fixture->dictionary));

  g_free (contents_path);
}


void
get_total_columns (Fixture * fixture, gconstpointer data)
{
  g_assert_cmpint (TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS, ==, lw_dictionary_total_columns (fixture->dictionary));
}


void
get_column_language_matches_known_values (Fixture * fixture, gconstpointer data)
{
    g_assert_cmpstr ("ja", ==, lw_dictionary_get_column_language (fixture->dictionary, LW_RADICALSDICTIONARYCOLUMNID_KANJI));
    g_assert_cmpstr ("ja", ==, lw_dictionary_get_column_language (fixture->dictionary, LW_RADICALSDICTIONARYCOLUMNID_RADICALS));
}


void
get_column_handling_matches_known_values (Fixture * fixture, gconstpointer data)
{
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary, LW_RADICALSDICTIONARYCOLUMNID_KANJI));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary, LW_RADICALSDICTIONARYCOLUMNID_RADICALS));
}


void
calculate_applicable_columns_for_text_when_english (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary, "English");

  g_assert_cmpint (-1, ==, columns[0]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_kanji (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary, "日本語");

  g_assert_cmpint (LW_RADICALSDICTIONARYCOLUMNID_KANJI, ==, columns[0]);
  g_assert_cmpint (LW_RADICALSDICTIONARYCOLUMNID_RADICALS, ==, columns[1]);
  g_assert_cmpint (-1, ==, columns[2]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_hiragana (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary, "にほんご");

  g_assert_cmpint (-1, ==, columns[0]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_kanjihiragana (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary, "生きます");

  g_assert_cmpint (-1, ==, columns[0]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_mix (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary, "日本語にほんごJapanese");

  g_assert_cmpint (-1, ==, columns[0]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_number (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary, "1");

  g_assert_cmpint (-1, ==, columns[0]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_blank (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary, "");

  g_assert_null (columns);

  g_free (columns);
}


void
ensure_cache_by_utf8flags (Fixture * fixture, gconstpointer data)
{
  LwDictionaryCache * cache = NULL;
  LwParsed * parsed = NULL;

  cache = lw_dictionary_ensure_cache_by_utf8flags (fixture->dictionary, LW_UTF8FLAG_NONE, NULL);
  parsed = lw_dictionarycache_get_parsed (cache);
  g_assert_nonnull (parsed);

  g_assert_cmpint (lw_parsed_num_lines (parsed), ==, 5);

  {
    LwParsedLine * line = lw_parsed_get_line (parsed, 0);
    g_assert_nonnull (line);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[0], ==, "亜");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[0], ==, "｜");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[1], ==, "一");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[2], ==, "口");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[3], ==, NULL);
  }

  {
    LwParsedLine * line = lw_parsed_get_line (parsed, 1);
    g_assert_nonnull (line);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[0], ==, "唖");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[0], ==, "｜");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[1], ==, "一");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[2], ==, "口");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[3], ==, NULL);
  }

  {
    LwParsedLine * line = lw_parsed_get_line (parsed, 2);
    g_assert_nonnull (line);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[0], ==, "娃");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[0], ==, "女");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[1], ==, "土");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[2], ==, NULL);
  }

  {
    LwParsedLine * line = lw_parsed_get_line (parsed, 3);
    g_assert_nonnull (line);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[0], ==, "阿");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_KANJI)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[0], ==, "一");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[1], ==, "口");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[2], ==, "亅");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[3], ==, "阡");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_RADICALSDICTIONARYCOLUMNID_RADICALS)[4], ==, NULL);
  }

  g_object_unref (cache);
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/get_contents_path", Fixture, NULL, setup, get_contents_path, teardown);

    g_test_add ("/get_id", Fixture, NULL, setup, get_id, teardown);

    g_test_add ("/get_name/when_unset", Fixture, NULL, setup, get_name_when_unset, teardown);

    g_test_add ("/set_name/to_null", Fixture, NULL, setup, set_name_to_null, teardown);
    g_test_add ("/set_name/to_something_else", Fixture, NULL, setup, set_name_to_something_else, teardown);

    g_test_add ("/get_contents", Fixture, NULL, setup, get_contents, teardown);
    g_test_add ("/get_contents_length", Fixture, NULL, setup, get_contents_length, teardown);

    g_test_add ("/set_contents_filename/to_null", Fixture, NULL, setup, set_contents_filename_to_null, teardown);
    g_test_add ("/set_contents_filename/to_something_else", Fixture, NULL, setup, set_contents_filename_to_something_else, teardown);

    g_test_add ("/get_total_columns", Fixture, NULL, setup, get_total_columns, teardown);

    g_test_add ("/get_column_language/matches_known_values", Fixture, NULL, setup, get_column_language_matches_known_values, teardown);

    g_test_add ("/get_column_handling/matches_known_values", Fixture, NULL, setup, get_column_handling_matches_known_values, teardown);

    g_test_add ("/calculate_applicable_columns_for_text/when_english", Fixture, NULL, setup, calculate_applicable_columns_for_text_when_english, teardown);
    g_test_add ("/calculate_applicable_columns_for_text/when_kanji", Fixture, NULL, setup, calculate_applicable_columns_for_text_when_kanji, teardown);
    g_test_add ("/calculate_applicable_columns_for_text/when_hiragana", Fixture, NULL, setup, calculate_applicable_columns_for_text_when_hiragana, teardown);
    g_test_add ("/calculate_applicable_columns_for_text/when_kanjihiragana", Fixture, NULL, setup, calculate_applicable_columns_for_text_when_kanjihiragana, teardown);
    g_test_add ("/calculate_applicable_columns_for_text/when_mix", Fixture, NULL, setup, calculate_applicable_columns_for_text_when_mix, teardown);
    g_test_add ("/calculate_applicable_columns_for_text/when_number", Fixture, NULL, setup, calculate_applicable_columns_for_text_when_number, teardown);
    g_test_add ("/calculate_applicable_columns_for_text/when_blank", Fixture, NULL, setup, calculate_applicable_columns_for_text_when_blank, teardown);

    g_test_add ("/ensure_cache_by_utf8flags", Fixture, NULL, setup, ensure_cache_by_utf8flags, teardown);

    return g_test_run ();
}
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarymodule.h>
#include <libwaei/dictionary/kanjidictionary.h>


static GTypeModule * module = NULL;

struct _Fixture {
  LwDictionary * dictionary;
  LwDictionaryClass * dictionary_class;
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
    module = lw_dictionarymodule_new (".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libkanjidictionary." G_MODULE_SUFFIX);
  }

  GType type = g_type_from_name ("LwKanjiDictionary");
  fixture->FILENAME = TESTDATADIR "kanjidictionary.data";
  fixture->dictionary = lw_dictionary_new (type, fixture->FILENAME);
  fixture->dictionary_class = LW_DICTIONARY_GET_CLASS (fixture->dictionary);
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

  g_assert_cmpstr (TESTDATADIR "kanjidictionary.data", ==, lw_dictionary_get_contents_path (fixture->dictionary));

  g_free (contents);
}


void
get_name_when_unset (Fixture * fixture, gconstpointer data)
{
  g_assert_cmpstr ("kanjidictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
}

void
get_id (Fixture * fixture, gconstpointer data)
{
  g_assert_cmpstr ("LwKanjiDictionary/kanjidictionary.data", ==, lw_dictionary_get_id (fixture->dictionary));
}

void
set_name_to_null (Fixture * fixture, gconstpointer data)
{
  lw_dictionary_set_name (fixture->dictionary, NULL);

  g_assert_cmpstr (NULL, ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr ("kanjidictionary.data", ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, <, lw_dictionary_contents_length (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents_checksum (fixture->dictionary));
  g_assert_cmpstr ("LwKanjiDictionary/kanjidictionary.data", ==, lw_dictionary_get_id (fixture->dictionary));
}


void
set_name_to_something_else (Fixture * fixture, gconstpointer data)
{
  lw_dictionary_set_name (fixture->dictionary, "new test name");

  g_assert_cmpstr ("new test name", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr ("kanjidictionary.data", ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, <, lw_dictionary_contents_length (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents_checksum (fixture->dictionary));
  g_assert_cmpstr ("LwKanjiDictionary/kanjidictionary.data", ==, lw_dictionary_get_id (fixture->dictionary));
}


void
set_contents_filename_to_null (Fixture * fixture, gconstpointer data)
{
  lw_dictionary_set_contents_filename (fixture->dictionary, NULL);

  g_assert_cmpstr ("kanjidictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
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
  gchar * contents_path = g_build_filename (fixture->datadir, "libwaei", "dictionary", "LwKanjiDictionary", "test new filename", NULL);

  lw_dictionary_set_contents_filename (fixture->dictionary, "test new filename");

  g_assert_cmpstr ("kanjidictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr ("test new filename", ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_cmpstr (contents_path, ==, lw_dictionary_get_contents_path (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, ==, lw_dictionary_contents_length (fixture->dictionary));
  g_assert_null (lw_dictionary_get_contents_checksum (fixture->dictionary));
  g_assert_cmpstr ("LwKanjiDictionary/test new filename", ==, lw_dictionary_get_id (fixture->dictionary));

  g_free (contents_path);
}


void
get_total_columns (Fixture * fixture, gconstpointer data)
{
  g_assert_cmpint (TOTAL_LW_KANJIDICTIONARYCOLUMNIDS, ==, lw_dictionary_total_columns (fixture->dictionary_class));
}


void
get_column_language_matches_known_values (Fixture * fixture, gconstpointer data)
{
    g_assert_cmpint (g_quark_from_string ("ja"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_KANJI));
    g_assert_cmpint (g_quark_from_string ("symbol"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL));
    g_assert_cmpint (g_quark_from_string ("number"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY));
    g_assert_cmpint (g_quark_from_string ("number"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT));
    g_assert_cmpint (g_quark_from_string ("number"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL));
    g_assert_cmpint (g_quark_from_string ("number"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL));
    g_assert_cmpint (g_quark_from_string ("ja"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS));
    g_assert_cmpint (g_quark_from_string ("ja"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_ON_READINGS));
    g_assert_cmpint (g_quark_from_string ("en"), ==, lw_dictionary_get_column_language (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_MEANINGS));
}


void
get_column_handling_matches_known_values (Fixture * fixture, gconstpointer data)
{
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_KANJI));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_ON_READINGS));
    g_assert_cmpint (LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH, ==, lw_dictionary_get_column_handling (fixture->dictionary_class, LW_KANJIDICTIONARYCOLUMNID_MEANINGS));
}


void
columnid_get_type (Fixture * fixture, gconstpointer data)
{
    GType type = lw_dictionary_get_columnid_type (fixture->dictionary_class);
    GType (* get_columnid_type) ();
    g_assert_true (lw_dictionarymodule_symbol (LW_DICTIONARYMODULE (module), "lw_kanjidictionary_get_columnid_type", (gpointer*) &get_columnid_type));
    g_assert_cmpint (type, ==, get_columnid_type());
    g_assert_true (g_type_is_a (type, G_TYPE_ENUM));
    GEnumClass * klass = G_ENUM_CLASS (g_type_class_ref (type));
    g_assert_cmpuint (klass->n_values, ==, TOTAL_LW_KANJIDICTIONARYCOLUMNIDS);
    g_type_class_unref (klass);
}


void
calculate_applicable_columns_for_text_when_english (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary_class, "English");

  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_MEANINGS, ==, columns[0]);
  g_assert_cmpint (-1, ==, columns[1]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_kanji (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary_class, "日本語");

  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_KANJI, ==, columns[0]);
  g_assert_cmpint (-1, ==, columns[1]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_hiragana (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary_class, "にほんご");

  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_ON_READINGS, ==, columns[0]);
  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS, ==, columns[1]);
  g_assert_cmpint (-1, ==, columns[2]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_kanjihiragana (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary_class, "生きます");

  g_assert_cmpint (-1, ==, columns[0]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_mix (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary_class, "日本語にほんごJapanese");

  g_assert_cmpint (-1, ==, columns[0]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_number (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary_class, "1");

  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY, ==, columns[0]);
  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT, ==, columns[1]);
  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL, ==, columns[2]);
  g_assert_cmpint (LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL, ==, columns[3]);
  g_assert_cmpint (-1, ==, columns[4]);

  g_free (columns);
}


void
calculate_applicable_columns_for_text_when_blank (Fixture * fixture, gconstpointer data)
{
  gint * columns = NULL;

  columns = lw_dictionary_calculate_applicable_columns_for_text (fixture->dictionary_class, "");

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

  g_assert_cmpint (lw_parsed_num_lines (parsed), ==, 9);

  {
    LwParsedLine * line = lw_parsed_get_line (parsed, 0);
    g_assert_nonnull (line);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_KANJI)[0], ==, "亜");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_KANJI)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL)[0], ==, "U4e9c");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY)[0], ==, "F1509");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT)[0], ==, "S7");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL)[0], ==, "G8");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL)[0], ==, "J1");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS)[0], ==, "ア");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_ON_READINGS)[0], ==, "つ.ぐ");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_ON_READINGS)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_MEANINGS)[0], ==, "-ous");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_MEANINGS)[1], ==, "come after");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_MEANINGS)[2], ==, "rank next");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_MEANINGS)[3], ==, "Asia");
    g_assert_cmpstr (lw_parsedline_get_strv (line, LW_KANJIDICTIONARYCOLUMNID_MEANINGS)[4], ==, NULL);
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

    g_test_add ("/columnid_get_type", Fixture, NULL, setup, columnid_get_type, teardown);

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

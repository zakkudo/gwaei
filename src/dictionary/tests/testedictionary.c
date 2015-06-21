#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarymodule.h>
#include <libwaei/dictionary/edictionary.h>


static GTypeModule * module = NULL;

struct _Fixture {
  LwDictionary * dictionary;
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
#ifdef OS_MINGW
  g_setenv("CSIDL_INTERNET_CACHE", fixture->cachedir, TRUE);
#else
  g_setenv("XDG_CACHE_HOME", fixture->cachedir, TRUE);
#endif

  fixture->datatmpl = g_build_filename (g_get_tmp_dir (), "testdatadir-XXXXXX", NULL);
  fixture->datadir = g_mkdtemp_full (fixture->datatmpl, 0700);
#ifdef OS_MINGW
  g_setenv("CSIDL_LOCAL_APPDATA", fixture->datadir, TRUE);
#else
  g_setenv("XDG_DATA_HOME", fixture->datadir, TRUE);
#endif

  if (module == NULL)
  {
    module = lw_dictionarymodule_new (".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libedictionary." G_MODULE_SUFFIX);
  }
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
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);

  gchar * contents = NULL;

  g_file_get_contents (FILENAME, &contents, NULL, NULL);

  g_assert_cmpstr (contents, ==, lw_dictionary_get_contents (fixture->dictionary));
  g_assert_nonnull (contents);
}


void
get_contents_length (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);

  gsize length = 0;
  gchar * contents = NULL;

  g_file_get_contents (FILENAME, &contents, &length, NULL);

  g_assert_cmpuint (length, ==, lw_dictionary_contents_length (fixture->dictionary));
}


void
get_contents_path (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);

  gsize length = 0;
  gchar * contents = NULL;

  g_assert_cmpstr (TESTDATADIR "edictionary.data", ==, lw_dictionary_get_contents_path (fixture->dictionary));
}


void
get_name_when_unset (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);

  g_assert_cmpstr ("edictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
}

void
set_name_to_null (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);

  lw_dictionary_set_name (fixture->dictionary, NULL);

  g_assert_cmpstr (NULL, ==, lw_dictionary_get_name (fixture->dictionary));
}


void
set_name_to_something_else (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);

  lw_dictionary_set_name (fixture->dictionary, "new test name");

  g_assert_cmpstr ("new test name", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_nonnull (lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, <, lw_dictionary_contents_length (fixture->dictionary));
}


void
set_contents_filename_to_null (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);

  lw_dictionary_set_contents_filename (fixture->dictionary, NULL);

  g_assert_cmpstr ("edictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents_path (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, ==, lw_dictionary_contents_length (fixture->dictionary));
}


void
set_contents_filename_to_something_else (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);
  gchar * contents_path = g_build_filename (fixture->datadir, "libwaei", "dictionary", "LwEDictionary", "test new filename", NULL);

  lw_dictionary_set_contents_filename (fixture->dictionary, "test new filename");

  g_assert_cmpstr ("edictionary.data", ==, lw_dictionary_get_name (fixture->dictionary));
  g_assert_cmpstr ("test new filename", ==, lw_dictionary_get_contents_filename (fixture->dictionary));
  g_assert_cmpstr (contents_path, ==, lw_dictionary_get_contents_path (fixture->dictionary));
  g_assert_cmpstr (NULL, ==, lw_dictionary_get_contents (fixture->dictionary));
  g_assert_cmpuint (0, ==, lw_dictionary_contents_length (fixture->dictionary));

  g_free (contents_path);
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/get_contents_path", Fixture, NULL, setup, get_contents_path, teardown);

    g_test_add ("/get_name/when_unset", Fixture, NULL, setup, get_name_when_unset, teardown);

    g_test_add ("/set_name/to_null", Fixture, NULL, setup, set_name_to_null, teardown);
    g_test_add ("/set_name/to_something_else", Fixture, NULL, setup, set_name_to_something_else, teardown);

    g_test_add ("/get_contents", Fixture, NULL, setup, get_contents, teardown);
    g_test_add ("/get_contents_length", Fixture, NULL, setup, get_contents_length, teardown);

    g_test_add ("/set_contents_filename/to_null", Fixture, NULL, setup, set_contents_filename_to_null, teardown);
    g_test_add ("/set_contents_filename/to_something_else", Fixture, NULL, setup, set_contents_filename_to_something_else, teardown);


    return g_test_run ();
}

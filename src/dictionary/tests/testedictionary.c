#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarymodule.h>
#include <libwaei/dictionary/edictionary.h>


struct _Fixture {
  GTypeModule * module;
  LwDictionary * dictionary;
  gchar * cachetmpl;
  gchar * cachedir;
  gchar * datatmpl;
  gchar * datadir;
};
typedef struct _Fixture Fixture;


void setup (Fixture *fixture, gconstpointer data)
{
  fixture->cachetmpl = g_strdup ("testcachedir-XXXXXX");
  fixture->cachedir = g_mkdtemp_full (fixture->cachetmpl, 0700);
#ifdef OS_MINGW
  g_setenv("CSIDL_INTERNET_CACHE", fixture->cachedir, TRUE);
#else
  g_setenv("XDG_CACHE_HOME", fixture->cachedir, TRUE);
#endif

  fixture->datatmpl = g_strdup ("testdatadir-XXXXXX");
  fixture->datadir = g_mkdtemp_full (fixture->datatmpl, 0700);
#ifdef OS_MINGW
  g_setenv("CSIDL_LOCAL_APPDATA", fixture->datadir, TRUE);
#else
  g_setenv("XDG_DATA_HOME", fixture->datadir, TRUE);
#endif

  fixture->module = lw_dictionarymodule_new (".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libedictionary." G_MODULE_SUFFIX);
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
test (Fixture * fixture, gconstpointer data)
{
  GType type = g_type_from_name ("LwEDictionary");
  gchar const * FILENAME = TESTDATADIR "edictionary.data";
  fixture->dictionary = lw_dictionary_new (type, FILENAME);
  
}

gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/test", Fixture, NULL, setup, test, teardown);


    return g_test_run ();
}

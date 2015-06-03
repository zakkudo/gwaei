#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarymodule.h>


static gchar const * LIBRARY_NAMES[] = {
  ".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libkanjidictionary." G_MODULE_SUFFIX,
  ".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libradicalsdictionary." G_MODULE_SUFFIX,
  ".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libunknowndictionary." G_MODULE_SUFFIX,
  ".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libexampledictionary." G_MODULE_SUFFIX,
  ".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libedictionary." G_MODULE_SUFFIX,
  NULL

};

struct _Fixture { LwDictionary * dictionary; GHashTable * library_names;};
typedef struct _Fixture Fixture;


void setup (Fixture *fixture, gconstpointer data)
{
  GHashTable * library_names = g_hash_table_new (g_str_hash, g_str_equal);
  gint i = 0;
  for (i = 0; LIBRARY_NAMES[i] != NULL; i++)
  {
    g_hash_table_add (library_names, (gpointer) LIBRARY_NAMES[i]);
  }
  fixture->library_names = library_names;
}

void teardown (Fixture *fixture, gconstpointer data)
{
  g_hash_table_unref (fixture->library_names);
  //if (fixture->dictionary != NULL) g_object_unref (fixture->dictionary);
}


void
dictionarymodulereader (Fixture * fixture, gconstpointer data)
{
  gchar * const OVERRIDE = ".." G_DIR_SEPARATOR_S ".libs";
  gchar * path = NULL;
  LwDictionaryModuleReader * reader = lw_dictionarymodulereader_open (OVERRIDE);

  while ((path = lw_dictionarymodulereader_read_path (reader)) != NULL)
  {
    g_assert_cmpstr (g_hash_table_lookup (fixture->library_names, path), ==, path);
  
    g_free (path);
    path = NULL;
  }
}


void
get_dictionarylib_searchpath_with_override (Fixture * fixture, gconstpointer data)
{
  gchar ** paths = NULL;
  gchar * override = NULL;

  override = g_strjoin (G_SEARCHPATH_SEPARATOR_S, "/test", "/override", NULL);
  paths = lw_dictionarymodule_get_dictionarylib_searchpath (override);

  g_assert_cmpstr (paths[0], ==, "/test");
  g_assert_cmpstr (paths[1], ==, "/override");
  g_assert_cmpstr (paths[2], ==, NULL);

  g_free (override);
  g_strfreev (paths);
}


void
get_dictionarylib_searchpath_with_default (Fixture * fixture, gconstpointer data)
{
  gchar ** paths = NULL;
  gchar * override = NULL;

  paths = lw_dictionarymodule_get_dictionarylib_searchpath (NULL);

  g_assert_cmpstr (paths[0], ==, DICTIONARYLIBDIR);
  g_assert_cmpstr (paths[1], ==, NULL);

  g_strfreev (paths);
}


void
get_dictionarylib_searchpath_with_env (Fixture * fixture, gconstpointer data)
{
  gchar ** paths = NULL;
  gchar * override = NULL;

  override = g_strjoin (G_SEARCHPATH_SEPARATOR_S, "/test", "/override", NULL);
  g_setenv ("DICTIONARYLIB_SEARCHPATH", override, TRUE);
  paths = lw_dictionarymodule_get_dictionarylib_searchpath (override);

  g_assert_cmpstr (paths[0], ==, "/test");
  g_assert_cmpstr (paths[1], ==, "/override");
  g_assert_cmpstr (paths[2], ==, NULL);

  g_free (override);
  g_strfreev (paths);
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/getdictionarylib_searchpath/with_default", Fixture, NULL, setup, get_dictionarylib_searchpath_with_default, teardown);
    g_test_add ("/getdictionarylib_searchpath/with_env", Fixture, NULL, setup, get_dictionarylib_searchpath_with_env, teardown);
    g_test_add ("/getdictionarylib_searchpath/with_override", Fixture, NULL, setup, get_dictionarylib_searchpath_with_override, teardown);

    g_test_add ("/dictionarymodulereader", Fixture, NULL, setup, dictionarymodulereader, teardown);


    return g_test_run ();
}

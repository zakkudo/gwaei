#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarymodule.h>


struct _Fixture { LwDictionary * dictionary;};
typedef struct _Fixture Fixture;


void setup (Fixture *fixture, gconstpointer data)
{
}

void teardown (Fixture *fixture, gconstpointer data)
{
  //if (fixture->dictionary != NULL) g_object_unref (fixture->dictionary);
}


void
dictionarymodulereader (Fixture * fixture, gconstpointer data)
{
  //LwDictionaryModuleReader * reader = lw_dictionarymodulereader_open ();
  //fixture->dictionary = lw_dictionary_new (LW_TYPE_EDICTIONARY, "test");
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

    g_test_add ("/test", Fixture, NULL, setup, dictionarymodulereader, teardown);


    return g_test_run ();
}

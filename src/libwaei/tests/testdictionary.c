#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>

#include "testdictionary.h"



struct _Fixture {
};
typedef struct _Fixture Fixture;


void setup (Fixture *fixture, gconstpointer data)
{
}


void teardown (Fixture *fixture, gconstpointer data)
{
    g_unsetenv ("CSIDL_LOCAL_APPDATA");
    g_unsetenv ("XDG_DATA_HOME");
    g_unsetenv ("DICTIONARYDATADIR");
}


void
get_install_directory (Fixture *fixture, gconstpointer data)
{
    if (g_test_subprocess ())
    {
      gchar * path = g_build_filename (g_get_user_data_dir (), "libwaei", "dictionary", NULL);

      g_assert_cmpstr (path, ==, lw_dictionary_get_install_directory ());

      g_free (path);

      return;
    }

    g_test_trap_subprocess (NULL, 0, 0);
    g_test_trap_assert_passed ();
}


void
get_install_directory_with_overridden_data_dir (Fixture *fixture, gconstpointer data)
{
    if (g_test_subprocess ())
    {
      gchar * path = g_build_filename (G_DIR_SEPARATOR_S "test", "data", "directory", "libwaei", "dictionary", NULL);
      g_setenv ("DICTIONARYDATADIR", "/test/data/directory", TRUE);

      g_assert_cmpstr (path, ==, lw_dictionary_get_install_directory ());

      g_free (path);

      return;
    }

    g_test_trap_subprocess (NULL, 0, 0);
    g_test_trap_assert_passed ();
}

void
get_install_directory_with_overridden_xdg_data_dir (Fixture *fixture, gconstpointer data)
{
    if (g_test_subprocess ())
    {
      gchar * path = g_build_filename (G_DIR_SEPARATOR_S "test", "data", "directory", "libwaei", "dictionary", NULL);

#ifdef OS_MINGW
      g_setenv ("CSIDL_LOCAL_APPDATA", "/test/data/directory", TRUE);
#else
      g_setenv ("XDG_DATA_HOME", "/test/data/directory", TRUE);
#endif

      g_assert_cmpstr (path, ==, lw_dictionary_get_install_directory ());

      g_free (path);

      return;
    }

    g_test_trap_subprocess (NULL, 0, 0);
    g_test_trap_assert_passed ();
}

void
build_contents_path_by_type_and_name_with_no_environmental_overrides (Fixture *fixture, gconstpointer data)
{
    if (g_test_subprocess ())
    {
      gchar * path = g_build_filename (g_get_user_data_dir (), "libwaei", "dictionary", "LwTestDictionary", "test name", NULL);

      g_assert_cmpstr(path, ==, lw_dictionary_build_contents_path_by_type_and_name (LW_TYPE_TESTDICTIONARY, "test name"));

      g_free (path);

      return;
    }

    g_test_trap_subprocess (NULL, 0, 0);
    g_test_trap_assert_passed ();
}

void
build_contents_path_by_type_and_name_with_overridden_data_dir (Fixture *fixture, gconstpointer data)
{
    if (g_test_subprocess ())
    {
      g_setenv ("DICTIONARYDATADIR", "/test/data/directory", TRUE);
      gchar * path = g_build_filename (G_DIR_SEPARATOR_S "test", "data", "directory", "libwaei", "dictionary", "LwTestDictionary", "test name", NULL);

      g_assert_cmpstr(path, ==, lw_dictionary_build_contents_path_by_type_and_name (LW_TYPE_TESTDICTIONARY, "test name"));

      g_free (path);

      return;
    }

    g_test_trap_subprocess (NULL, 0, 0);
    g_test_trap_assert_passed ();
}

void
build_contents_path_by_type_and_name_with_overridden_xdg_data_dir (Fixture *fixture, gconstpointer data)
{
    if (g_test_subprocess ())
    {
#ifdef OS_MINGW
      g_setenv ("CSIDL_LOCAL_APPDATA", "/test/data/directory", TRUE);
#else
      g_setenv ("XDG_DATA_HOME", "/test/data/directory", TRUE);
#endif
      gchar * path = g_build_filename (G_DIR_SEPARATOR_S "test", "data", "directory", "libwaei", "dictionary", "LwTestDictionary", "test name", NULL);

      g_assert_cmpstr(path, ==, lw_dictionary_build_contents_path_by_type_and_name (LW_TYPE_TESTDICTIONARY, "test name"));
      g_free (path);

      return;
    }

    g_test_trap_subprocess (NULL, 0, 0);
    g_test_trap_assert_passed ();
}

gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/get_install_directory/default_env", Fixture, NULL, setup, get_install_directory, teardown);
    g_test_add ("/get_install_directory/with_overridden_data_dir", Fixture, NULL, setup, get_install_directory_with_overridden_data_dir, teardown);
    g_test_add ("/get_install_directory/with_overridden_xdg_data_dir", Fixture, NULL, setup, get_install_directory_with_overridden_data_dir, teardown);

    g_test_add ("/build_contents_path_by_type_and_name/with_no_environmental_overrides", Fixture, NULL, setup, build_contents_path_by_type_and_name_with_no_environmental_overrides, teardown);
    g_test_add ("/build_contents_path_by_type_and_name/with_overridden_data_dir", Fixture, NULL, setup, build_contents_path_by_type_and_name_with_overridden_data_dir, teardown);
    g_test_add ("/build_contents_path_by_type_and_name/with_overridden_xdg_data_dir", Fixture, NULL, setup, build_contents_path_by_type_and_name_with_overridden_xdg_data_dir, teardown);

    return g_test_run();
}

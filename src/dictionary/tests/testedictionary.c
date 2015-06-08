#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionarymodule.h>
#include <libwaei/dictionary/edictionary.h>


struct _Fixture { GTypeModule * module; LwDictionary * dictionary;};
typedef struct _Fixture Fixture;


void setup (Fixture *fixture, gconstpointer data)
{
  //fixture->module = lw_dictionarymodule_new (".." G_DIR_SEPARATOR_S ".libs" G_DIR_SEPARATOR_S "libedictionary." G_MODULE_SUFFIX);
  fixture->module = lw_dictionarymodule_new (G_DIR_SEPARATOR_S "usr" G_DIR_SEPARATOR_S "local" G_DIR_SEPARATOR_S "lib" G_DIR_SEPARATOR_S "gwaei" G_DIR_SEPARATOR_S "dictionary" G_DIR_SEPARATOR_S "libedictionary." G_MODULE_SUFFIX);
  GType type = g_type_from_name ("LwEDictionary");
  //fixture->dictionary = lw_dictionary_new (type, "fish.txt");
}

void teardown (Fixture *fixture, gconstpointer data)
{
  //if (fixture->dictionary != NULL) g_object_unref (fixture->dictionary);
}


void
test (Fixture * fixture, gconstpointer data)
{
  //fixture->dictionary = lw_dictionary_new (LW_TYPE_EDICTIONARY, "test");
}

gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/test", Fixture, NULL, setup, test, teardown);


    return g_test_run ();
}

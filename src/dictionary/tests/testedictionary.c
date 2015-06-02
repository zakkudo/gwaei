#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/dictionary.h>
#include <libwaei/dictionary/edictionary.h>


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

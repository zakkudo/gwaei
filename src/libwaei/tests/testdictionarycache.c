#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

struct _Fixture {
  LwDictionaryCache * cache;
};
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
  //fixture->cache = lw_dictionarycache_new (NAME, flags);
}


void teardown (Fixture *fixture, gconstpointer data)
{
  g_object_unref (fixture->cache);
}


void
test (Fixture       * fixture,
      gconstpointer   data)
{
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/test", Fixture, NULL, setup, test, teardown);

    return g_test_run();
}






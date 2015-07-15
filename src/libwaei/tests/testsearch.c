#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/utf8.h>


struct _Fixture { LwProgress * progress; GArray *steps;};
typedef struct _Fixture Fixture;

static void
append_progress (LwProgress * self, Fixture * fixture)
{
  gint percent = (gint) (lw_progress_get_fraction (self) * 100.0);
  g_array_append_val (fixture->steps, percent);
}


static void
cancel_progress (LwProgress *self, Fixture * fixture)
{
  if (lw_progress_get_fraction (self) > .5)
  {
    lw_progress_cancel (self);
  }
}


void setup (Fixture *fixture, gconstpointer data)
{
    fixture->progress = lw_progress_new ();
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (append_progress), fixture);
    lw_progress_set_prefered_chunk_size (fixture->progress, 2);
    fixture->steps = g_array_sized_new (FALSE, TRUE, sizeof(gint), 20);
}

void teardown (Fixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->progress);
    fixture->progress = NULL;
    g_array_free (fixture->steps, TRUE);
    fixture->steps = NULL;
}


void
dictionary (Fixture *fixture, gconstpointer data)
{
  /*TODO
    //Declarations
    LwSearch * search = NULL;
    GError * error = NULL;

    search = lw_search_new ("test", dictionary, 0);

    lw_search_dictionary (search, FALSE, &error);

    g_object_unref (search);
    */
}





gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/dictionary", Fixture, NULL, setup, dictionary, teardown);

    return g_test_run ();
}

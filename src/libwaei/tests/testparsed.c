#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/parsed.h>


struct _Fixture { LwParsed parsed; };
typedef struct _Fixture Fixture;


static gchar const * CONTENTS = "語彙\0ごい\0word\0vocabulary\0vocab";

void setup (Fixture *fixture, gconstpointer data)
{
}


void teardown (Fixture *fixture, gconstpointer data)
{
}


void
take_strv_values_for_multiple_columns (Fixture *fixture, gconstpointer data)
{
}

gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    //g_test_add ("/take_strv/values_for_multiple_columns", Fixture, NULL, setup, take_strv_values_for_multiple_columns, teardown);

    return g_test_run();
}

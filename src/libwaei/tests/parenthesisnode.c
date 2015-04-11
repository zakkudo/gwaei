#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

/*
  Methods tested

  lw_index_append_data_offset
  lw_index_get_data_offsets
  lw_index_get_data_offsets_length
  lw_index_search
  lw_index_data_is_valid
*/

struct _Fixture { gint unused; };
typedef struct _Fixture Fixture;
void index_test_setup (Fixture *fixture, gconstpointer data) {}
void index_test_teardown (Fixture *fixture, gconstpointer data) {}


void
parse_no_parenthesis_string (Fixture *fixture, gconstpointer data)
{
/*TODO
    //Arrange
    fixture->index = lw_index_new (fixture->engine);

    gint expected_offsets[] = { 10, 20, 35 };
    gint expected_length = G_N_ELEMENTS (expected_offsets);
    gint i = 0;
    const gchar *KEY = "test";

    for (i = 0; i < expected_length; i++)
    {
      lw_index_append_data_offset (fixture->index, KEY, expected_offsets[i]);
    }

    //Act
    LwOffsets* actual_offsets = lw_index_get_data_offsets (fixture->index, KEY);
    gint actual_length = lw_index_get_data_offsets_length (fixture->index, KEY);

    //Assert
    g_assert_cmpuint (expected_length, ==, actual_length);

    for (i = 0; i < expected_length; i++)
    {
      g_assert_cmpuint (expected_offsets[i], ==, actual_offsets[i]);
    }

    lw_index_free (fixture->index);
*/
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/libwaei/parenthesisnode/parse_no_parenthesis_string", Fixture, NULL, NULL, parse_no_parenthesis_string, NULL);

    return g_test_run();
}



#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "results.h"

#include "testdictionary.h"


struct _Fixture {
  LwResults * results;
  LwDictionaryCache * dictionary_cache;
  LwParsedLine line;
  GList * rows_reordered;
};
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->dictionary_cache = lw_dictionarycache_new ("test", LW_TYPE_TESTDICTIONARY, 0);
    fixture->results = lw_results_new (fixture->dictionary_cache);
    fixture->rows_reordered;
}

void teardown (Fixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->results);
    g_object_unref (fixture->dictionary_cache);
    g_list_free_full (fixture->rows_reordered, (GDestroyNotify) g_free);
}


void
new (Fixture *fixture, gconstpointer data)
{
  g_assert (fixture->dictionary_cache == lw_results_get_dictionarycache (fixture->results));
  g_assert_cmpint (0, ==, g_sequence_get_length (lw_results_get_sequence (fixture->results)));
}


void
append_result (Fixture *fixture, gconstpointer data)
{
    GSequence * sequence = lw_results_get_sequence (fixture->results);
    LwResult * result = lw_result_new (1);

    lw_results_append_result (fixture->results, result);

    g_assert (fixture->dictionary_cache == lw_results_get_dictionarycache (fixture->results));
    g_assert_cmpint (1, ==, g_sequence_get_length (sequence));
    g_assert_cmpint (result->line_number, ==, 1);
    g_assert_cmpint (result->index, ==, 0);
}


static void
_rows_reordered (LwResults * results,
                 gint      * new_order,
                 Fixture   * fixture)
{
  gint length = lw_results_length (results);
  new_order = g_memdup (new_order, sizeof(gint) * length);
  fixture->rows_reordered = g_list_prepend (fixture->rows_reordered, new_order);
}


void
sort_by_score (Fixture * fixture, gconstpointer data)
{
    //Arrange
    LwResult * result = NULL;
    gint i;
    gint expected_new_order[] = {1, 3, 0, 2};
    g_signal_connect (G_OBJECT (fixture->results), "internal-rows-reordered", G_CALLBACK (_rows_reordered), fixture);

    result = lw_result_new (1);
    result->score = 10;
    lw_results_append_result (fixture->results, result);

    result = lw_result_new (2);
    result->score = 5;
    lw_results_append_result (fixture->results, result);

    result = lw_result_new (3);
    result->score = 20;
    lw_results_append_result (fixture->results, result);

    result = lw_result_new (5);
    result->score = 7;
    lw_results_append_result (fixture->results, result);

    //Act
    lw_results_sort_by_score (fixture->results);

    //Assert
    for (i = 0; i < G_N_ELEMENTS (expected_new_order); i++)
    {
        gint * new_order = (gint*) fixture->rows_reordered->data;
        g_assert_cmpint (expected_new_order[i], ==, new_order[i]);
    }

    {
      GSequence * sequence = lw_results_get_sequence (fixture->results);
      g_assert_cmpint (g_sequence_get_length (sequence), ==, 4);
      GSequenceIter * iter = g_sequence_get_begin_iter (sequence);
      result = g_sequence_get (iter);
      g_assert_cmpint (result->index, ==, 0);
      g_assert_cmpint (result->line_number, ==, 2);
      g_assert_cmpint (result->score, ==, 5);
      
      iter = g_sequence_iter_next (iter);
      result = g_sequence_get (iter);
      g_assert_cmpint (result->index, ==, 1);
      g_assert_cmpint (result->line_number, ==, 5);
      g_assert_cmpint (result->score, ==, 7);

      iter = g_sequence_iter_next (iter);
      result = g_sequence_get (iter);
      g_assert_cmpint (result->index, ==, 2);
      g_assert_cmpint (result->line_number, ==, 1);
      g_assert_cmpint (result->score, ==, 10);

      iter = g_sequence_iter_next (iter);
      result = g_sequence_get (iter);
      g_assert_cmpint (result->index, ==, 3);
      g_assert_cmpint (result->line_number, ==, 3);
      g_assert_cmpint (result->score, ==, 20);
    }
}


void
sort_by_index (Fixture * fixture, gconstpointer data)
{
}


void
sort_by_columnid (Fixture * fixture, gconstpointer data)
{
}




gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/new", Fixture, NULL, setup, new, teardown);
    g_test_add ("/append_result", Fixture, NULL, setup, append_result, teardown);
    g_test_add ("/sort_by_score", Fixture, NULL, setup, sort_by_score, teardown);

    return g_test_run ();
}


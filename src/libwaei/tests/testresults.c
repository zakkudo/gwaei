#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/results.h>

#include "testdictionary.h"


struct _Fixture {LwResults * results; LwDictionaryCache * dictionary_cache; LwParsedLine line;};
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->dictionary_cache = lw_dictionarycache_new ("test", LW_TYPE_TESTDICTIONARY, 0);
    fixture->results = lw_results_new (fixture->dictionary_cache);
}

void teardown (Fixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->results);
    g_object_unref (fixture->dictionary_cache);
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


void
sort_by_score (Fixture * fixture, gconstpointer data)
{
    //Arrange
    LwResult * result = NULL;

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

    //Asset
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


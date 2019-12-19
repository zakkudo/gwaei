#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "querynodecolumnmatchinfo.h"
#include "querynodematchmarker.h"


struct _Fixture {LwQueryNodeColumnMatchInfo *column_match_info;};
typedef struct _Fixture Fixture;

static gchar const * reading[] = {
  "Japanese",
  "English",
  "French",
  NULL
};

void setup (Fixture *fixture, gconstpointer data)
{
  fixture->column_match_info = lw_querynodecolumnmatchinfo_new (2, reading);
}

void teardown (Fixture *fixture, gconstpointer data)
{
  lw_querynodecolumnmatchinfo_unref (fixture->column_match_info);
}


void
get_column (Fixture *fixture, gconstpointer data)
{
    g_assert_cmpint (lw_querynodecolumnmatchinfo_get_column (fixture->column_match_info), ==, 2);
}


void
add_valid_match (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("glis", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[1], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  LwQueryNodeMatchMarker * marker = NULL;
  GList * link = NULL;
  LwQueryNodeMatchMarkerType type = 0;
  gchar const * position = NULL;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  link = fixture->column_match_info->markers;
  marker = LW_QUERYNODEMATCHMARKER (link->data);
  position = lw_querynodematchmarker_get_position (marker, &type);

  g_assert_cmpint (type, ==, LW_QUERYNODEMATCHMARKERTYPE_CLOSE);
  g_assert (position == reading[1] + strlen("Englis"));

  link = link->next;
  marker = LW_QUERYNODEMATCHMARKER (link->data);
  position = lw_querynodematchmarker_get_position (marker, &type);

  g_assert_cmpint (type, ==, LW_QUERYNODEMATCHMARKERTYPE_OPEN);
  g_assert (position == reading[1] + strlen("En"));

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


void
add_non_matching (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("invalid", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[1], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  LwQueryNodeMatchMarker * marker = NULL;
  GList * link = NULL;
  LwQueryNodeMatchMarkerType type = 0;
  gchar const * position = NULL;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  g_assert_null (fixture->column_match_info->markers);

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


void
read_start_match (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("Eng", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[1], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "Japanese");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "English");
  g_assert_cmpstr (end, ==, "lish");
  g_assert_true (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "lish");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 2);
  g_assert_cmpstr (start, ==, "French");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_false (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_false (has_more);

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


void
read_middle_match (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("ngli", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[1], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "Japanese");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "English");
  g_assert_cmpstr (end, ==, "nglish");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "nglish");
  g_assert_cmpstr (end, ==, "sh");
  g_assert_true (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "sh");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 2);
  g_assert_cmpstr (start, ==, "French");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_false (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_false (has_more);

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


void
read_end_match (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("sh", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[1], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "Japanese");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "English");
  g_assert_cmpstr (end, ==, "sh");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "sh");
  g_assert_cmpstr (end, ==, "");
  g_assert_true (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 2);
  g_assert_cmpstr (start, ==, "French");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_false (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_false (has_more);

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


void
read_overlapping_matches (Fixture *fixture, gconstpointer data)
{
  GRegex * regex1 = g_regex_new ("ngl", G_REGEX_OPTIMIZE, 0, NULL);
  GRegex * regex2 = g_regex_new ("lis", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info1 = NULL;
  GMatchInfo * match_info2 = NULL;
  gboolean matches = g_regex_match (regex1, reading[1], 0, &match_info1);
  matches = g_regex_match (regex2, reading[1], 0, &match_info2);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info1);
  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info2);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "Japanese");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "English");
  g_assert_cmpstr (end, ==, "nglish");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "nglish");
  g_assert_cmpstr (end, ==, "h");
  g_assert_true (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "h");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 2);
  g_assert_cmpstr (start, ==, "French");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_false (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_false (has_more);

  g_match_info_unref (match_info1);
  match_info1 = NULL;
  g_match_info_unref (match_info2);
  match_info2 = NULL;
  g_regex_unref (regex1);
  regex1 = NULL;
  g_regex_unref (regex2);
  regex2 = NULL;
}


void
read_duplicate_matches (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("sh", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[1], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);
  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);
  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);
  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "Japanese");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "English");
  g_assert_cmpstr (end, ==, "sh");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "sh");
  g_assert_cmpstr (end, ==, "");
  g_assert_true (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 2);
  g_assert_cmpstr (start, ==, "French");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_false (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_false (has_more);

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


void
read_multiple_matches (Fixture *fixture, gconstpointer data)
{
  GRegex * regex1 = g_regex_new ("J", G_REGEX_OPTIMIZE, 0, NULL);
  GRegex * regex2 = g_regex_new ("pan", G_REGEX_OPTIMIZE, 0, NULL);
  GRegex * regex3 = g_regex_new ("French", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info1 = NULL;
  GMatchInfo * match_info2 = NULL;
  GMatchInfo * match_info3 = NULL;
  gboolean matches = g_regex_match (regex1, reading[0], 0, &match_info1);
  matches = g_regex_match (regex2, reading[0], 0, &match_info2);
  matches = g_regex_match (regex3, reading[2], 0, &match_info3);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info1);
  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info2);
  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info3);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "Japanese");
  g_assert_cmpstr (end, ==, "apanese");
  g_assert_true (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "apanese");
  g_assert_cmpstr (end, ==, "panese");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "panese");
  g_assert_cmpstr (end, ==, "ese");
  g_assert_true (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "ese");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "English");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 2);
  g_assert_cmpstr (start, ==, "French");
  g_assert_cmpstr (end, ==, "");
  g_assert_true (is_match);
  g_assert_false (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_false (has_more);

  g_match_info_unref (match_info1);
  match_info1 = NULL;
  g_match_info_unref (match_info2);
  match_info2 = NULL;
  g_match_info_unref (match_info3);
  match_info3 = NULL;
  g_regex_unref (regex1);
  regex1 = NULL;
  g_regex_unref (regex2);
  regex2 = NULL;
  g_regex_unref (regex3);
  regex3 = NULL;
}


void
read_non_matching (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("Chinese", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[0], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gint i = 0;
  gchar const * start = NULL;
  gchar const * end = NULL;
  gboolean is_match = FALSE;
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 0);
  g_assert_cmpstr (start, ==, "Japanese");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 1);
  g_assert_cmpstr (start, ==, "English");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_cmpint (i, ==, 2);
  g_assert_cmpstr (start, ==, "French");
  g_assert_cmpstr (end, ==, "");
  g_assert_false (is_match);
  g_assert_false (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, &i, &start, &end, &is_match);
  g_assert_false (has_more);

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


void
read_with_null_out (Fixture *fixture, gconstpointer data)
{
  GRegex * regex = g_regex_new ("gli", G_REGEX_OPTIMIZE, 0, NULL);
  GMatchInfo * match_info = NULL;
  gboolean matches = g_regex_match (regex, reading[1], 0, &match_info);
  LwQueryNodeColumnMatchInfoIter iter = {0};
  gboolean has_more = FALSE;

  lw_querynodecolumnmatchinfo_add (fixture->column_match_info, match_info);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, NULL, NULL, NULL, NULL);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, NULL, NULL, NULL, NULL);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, NULL, NULL, NULL, NULL);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, NULL, NULL, NULL, NULL);
  g_assert_true (has_more);

  has_more = lw_querynodecolumnmatchinfo_read (fixture->column_match_info, &iter, NULL, NULL, NULL, NULL);
  g_assert_false (has_more);

  g_match_info_unref (match_info);
  match_info = NULL;
  g_regex_unref (regex);
  regex = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/get_column", Fixture, NULL, setup, get_column, teardown);

    g_test_add ("/add/valid_match", Fixture, NULL, setup, add_valid_match, teardown);
    g_test_add ("/add/non_matching", Fixture, NULL, setup, add_non_matching, teardown);

    g_test_add ("/read/start_match", Fixture, NULL, setup, read_start_match, teardown);
    g_test_add ("/read/middle_match", Fixture, NULL, setup, read_middle_match, teardown);
    g_test_add ("/read/end_match", Fixture, NULL, setup, read_end_match, teardown);
    g_test_add ("/read/overlapping_matches", Fixture, NULL, setup, read_overlapping_matches, teardown);
    g_test_add ("/read/duplicate_matches", Fixture, NULL, setup, read_duplicate_matches, teardown);
    g_test_add ("/read/multiple_matches", Fixture, NULL, setup, read_multiple_matches, teardown);
    g_test_add ("/read/non_matching", Fixture, NULL, setup, read_non_matching, teardown);
    g_test_add ("/read/with_null_out", Fixture, NULL, setup, read_with_null_out, teardown);

    return g_test_run ();
}

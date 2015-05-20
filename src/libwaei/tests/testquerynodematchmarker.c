#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

struct _Fixture {
  LwQueryNodeMatchMarker * marker;
};
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
}

void teardown (Fixture *fixture, gconstpointer data)
{
  if (fixture->marker != NULL)
    lw_querynodematchmarker_unref (fixture->marker);
}


void
new_open_marker (Fixture       * fixture,
                 gconstpointer   data)
{
    //Arrange
    gchar const * CONTENT = "test string";
    GRegex * regex = NULL;
    gboolean matches = NULL;
    GMatchInfo * match_info = NULL;
    LwQueryNodeMatchMarkerType type = 0;


    //Act
    regex = g_regex_new ("st st", 0, 0, NULL);
    matches = g_regex_match (regex, CONTENT, 0, &match_info);
    fixture->marker = lw_querynodematchmarker_new (LW_QUERYNODEMATCHMARKERTYPE_OPEN, match_info);

    //Assert
    g_assert_cmpstr (lw_querynodematchmarker_get_position (fixture->marker, &type), ==, "st string");
    g_assert_cmpstr (lw_querynodematchmarker_get_string (fixture->marker), ==, "test string");
    g_assert_cmpint (type, ==, LW_QUERYNODEMATCHMARKERTYPE_OPEN);
    g_assert (lw_querynodematchmarker_get_regex (fixture->marker) == regex);

    lw_querynodematchmarker_unref (fixture->marker);
    fixture->marker = NULL;
    g_regex_unref (regex);
    regex = NULL;
    g_match_info_unref (match_info);
    match_info = NULL;
}


void
new_close_marker (Fixture      * fixture,
                 gconstpointer   data)
{
    //Arrange
    gchar const * CONTENT = "test string";
    GRegex * regex = NULL;
    gboolean matches = NULL;
    GMatchInfo * match_info = NULL;
    LwQueryNodeMatchMarkerType type = 0;

    //Act
    regex = g_regex_new ("st st", 0, 0, NULL);
    matches = g_regex_match (regex, CONTENT, 0, &match_info);
    fixture->marker = lw_querynodematchmarker_new (LW_QUERYNODEMATCHMARKERTYPE_CLOSE, match_info);

    //Assert
    g_assert_cmpstr (lw_querynodematchmarker_get_position (fixture->marker, &type), ==, "ring");
    g_assert_cmpstr (lw_querynodematchmarker_get_string (fixture->marker), ==, "test string");
    g_assert_cmpint (type, ==, LW_QUERYNODEMATCHMARKERTYPE_CLOSE);
    g_assert (lw_querynodematchmarker_get_regex (fixture->marker) == regex);

    lw_querynodematchmarker_unref (fixture->marker);
    fixture->marker = NULL;
    g_regex_unref (regex);
    regex = NULL;
    g_match_info_unref (match_info);
    match_info = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/new/open_marker", Fixture, NULL, setup, new_open_marker, teardown);
    g_test_add ("/new/close_marker", Fixture, NULL, setup, new_close_marker, teardown);

    return g_test_run();
}




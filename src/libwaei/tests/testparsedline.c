#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/parsedline.h>


struct _Fixture { GList * strvs; LwParsedLine parsed_line; };
typedef struct _Fixture Fixture;


static gchar const * CONTENTS = "語彙\0ごい\0word\0vocabulary\0vocab";
static gchar const * INVALID_CONTENTS = "\xF0彙\0ごい\0word\0vocabulary\0vocab";
static gchar const * SWAPPED_CONTENTS = "aaa彙\0ごい\0word\0vocabulary\0vocab";

void setup (Fixture *fixture, gconstpointer data)
{
    lw_parsedline_init_full (&fixture->parsed_line, (GDestroyNotify) g_free);

    {
      gchar ** buffer = g_new0(gchar*, 2);
      buffer[0] = (gchar*) CONTENTS;
      lw_parsedline_set_strv (&fixture->parsed_line, 1, buffer);
    }

    {
      gchar ** buffer = g_new0(gchar*, 2);
      buffer[0] = (gchar*) CONTENTS + strlen("語彙 ");
      lw_parsedline_set_strv (&fixture->parsed_line, 2, buffer);
    }

    {
      gchar ** buffer = g_new0(gchar*, 4);
      buffer[0] = (gchar*) CONTENTS + strlen("語彙 ごい ");
      buffer[1] = (gchar*) CONTENTS + strlen("語彙 ごい word ");
      buffer[2] = (gchar*) CONTENTS + strlen("語彙 ごい word vocabulary ");
      lw_parsedline_set_strv (&fixture->parsed_line, 3, buffer);
    }
}


void teardown (Fixture *fixture, gconstpointer data)
{
  lw_parsedline_clear (&fixture->parsed_line);
}


void
take_strv_values_for_multiple_columns (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine * parsed_line = &fixture->parsed_line;

    //Initializations

    //Assert
    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 1)[0], ==, "語彙");
    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 1)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 2)[0], ==, "ごい");
    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 2)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 3)[0], ==, "word");
    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 3)[1], ==, "vocabulary");
    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 3)[2], ==, "vocab");
    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 3)[3], ==, NULL);
}


void
take_strv_with_no_destroy_notify_function (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine parsed_line = {0};
    lw_parsedline_init (&parsed_line);
    gchar const * WORD = "語彙";
    gchar const * READINGS = "ごい";
    gchar const * DEFINITIONS = "word;vocabulary;vocab";
    gchar ** words = NULL;
    gchar ** readings = NULL;
    gchar ** definitions = NULL;

    //Initializations
    words = g_strsplit (WORD, ";", -1);
    readings = g_strsplit (READINGS, ";", -1);
    definitions = g_strsplit (DEFINITIONS, ";", -1);

    lw_parsedline_set_strv (&parsed_line, 1, words);
    lw_parsedline_set_strv (&parsed_line, 2, readings);
    lw_parsedline_set_strv (&parsed_line, 3, definitions);

    //Assert
    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 1)[0], ==, "語彙");
    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 1)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 2)[0], ==, "ごい");
    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 2)[1], ==, NULL);

    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 3)[0], ==, "word");
    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 3)[1], ==, "vocabulary");
    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 3)[2], ==, "vocab");
    g_assert_cmpstr (lw_parsedline_get_strv(&parsed_line, 3)[3], ==, NULL);

    g_strfreev (words);
    words = NULL;
    g_strfreev (readings);
    readings = NULL;
    g_strfreev (definitions);
    definitions = NULL;

    lw_parsedline_clear (&parsed_line);
}


void
get_strv_with_nonexisting_column (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine * parsed_line = &fixture->parsed_line;

    //Initializations

    //Assert
    g_assert_cmpstr (lw_parsedline_get_strv(parsed_line, 1)[4], ==, NULL);
}


void
get_serialized_length (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine * parsed_line = &fixture->parsed_line;
    gsize length = 0;

    length = lw_parsedline_get_serialized_length (parsed_line);

    //Assert
}


void
serialize (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwParsedLine other_parsed_line = {0};
    lw_parsedline_init (&other_parsed_line);
    gsize length = 0;
    gsize written = 0;
    gsize read = 0;
    gchar * preallocated_buffer = NULL;
    GError * error = NULL;

    length = lw_parsedline_get_serialized_length (parsed_line);
    preallocated_buffer = g_new0 (gchar, length);
    written = lw_parsedline_serialize (parsed_line, CONTENTS, preallocated_buffer, &error);
    read = lw_parsedline_deserialize_into (&other_parsed_line, preallocated_buffer, CONTENTS, &error);

    //Assert
    g_assert_cmpuint (length, ==, written);
    g_assert_cmpuint (length, ==, read);
    lw_parsedline_assert_equals (parsed_line, &other_parsed_line);
    g_assert_no_error (error);

    lw_parsedline_clear (&other_parsed_line);
    g_free (preallocated_buffer);
    preallocated_buffer = NULL;
}


void
deserialize_twice_on_same_data (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwParsedLine other_parsed_line = {0};
    LwParsedLine duplicate_other_parsed_line = {0};
    gsize length = 0;
    gsize written = 0;
    gsize read = 0;
    gchar * preallocated_buffer = NULL;
    GError * error = NULL;

    length = lw_parsedline_get_serialized_length (parsed_line);
    preallocated_buffer = g_new0 (gchar, length);
    written = lw_parsedline_serialize (parsed_line, CONTENTS, preallocated_buffer, &error);
    read = lw_parsedline_deserialize_into (&other_parsed_line, preallocated_buffer, CONTENTS, &error);
    read = lw_parsedline_deserialize_into (&duplicate_other_parsed_line, preallocated_buffer, CONTENTS, &error);

    //Assert
    g_assert_cmpuint (length, ==, written);
    g_assert_cmpuint (length, !=, read);
    g_assert_error (error, LW_PARSEDLINE_ERROR, LW_PARSEDLINE_ERRORCODE_DESERIALIZATION_ERROR);

    lw_parsedline_clear (&other_parsed_line);
    lw_parsedline_clear (&duplicate_other_parsed_line);
    g_free (preallocated_buffer);
    preallocated_buffer = NULL;
    g_clear_error (&error);
}


void
deserialize_invalid_utf8_data (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwParsedLine other_parsed_line = {0};
    gsize length = 0;
    gsize written = 0;
    gsize read = 0;
    gchar * preallocated_buffer = NULL;
    GError * error = NULL;

    length = lw_parsedline_get_serialized_length (parsed_line);
    preallocated_buffer = g_new0 (gchar, length);
    written = lw_parsedline_serialize (parsed_line, CONTENTS, preallocated_buffer, &error);
    read = lw_parsedline_deserialize_into (&other_parsed_line, preallocated_buffer, INVALID_CONTENTS, &error);

    //Assert
    g_assert_cmpuint (length, ==, written);
    g_assert_cmpuint (length, >, read);
    g_assert_error (error, LW_PARSEDLINE_ERROR, LW_PARSEDLINE_ERRORCODE_DESERIALIZATION_ERROR);

    lw_parsedline_clear (&other_parsed_line);
    g_free (preallocated_buffer);
    preallocated_buffer = NULL;
    g_clear_error (&error);
}


void
deserialize_swapped_but_valid_contents (Fixture *fixture, gconstpointer data)
{
    //Declarations
    LwParsedLine * parsed_line = &fixture->parsed_line;
    LwParsedLine other_parsed_line = {0};
    gsize length = 0;
    gsize written = 0;
    gsize read = 0;
    gchar * preallocated_buffer = NULL;
    GError * error = NULL;

    length = lw_parsedline_get_serialized_length (parsed_line);
    preallocated_buffer = g_new0 (gchar, length);
    written = lw_parsedline_serialize (parsed_line, CONTENTS, preallocated_buffer, &error);
    read = lw_parsedline_deserialize_into (&other_parsed_line, preallocated_buffer, SWAPPED_CONTENTS, &error);

    //Assert
    g_assert_cmpuint (length, ==, written);
    g_assert_cmpuint (length, ==, read);
    g_assert_no_error (error);

    g_assert_cmpstr (lw_parsedline_get_strv (parsed_line, 1)[0], !=, lw_parsedline_get_strv (&other_parsed_line, 1)[0]);

    lw_parsedline_clear (&other_parsed_line);
    g_free (preallocated_buffer);
    preallocated_buffer = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/take_strv/values_for_multiple_columns", Fixture, NULL, setup, take_strv_values_for_multiple_columns, teardown);
    g_test_add ("/take_strv/with_no_destory_notify_function", Fixture, NULL, setup, take_strv_with_no_destroy_notify_function, teardown);

    g_test_add ("/get_strv/with_nonexisting_column", Fixture, NULL, setup, get_strv_with_nonexisting_column, teardown);

    g_test_add ("/get_serialized_length", Fixture, NULL, setup, get_serialized_length, teardown);

    g_test_add ("/serialize", Fixture, NULL, setup, serialize, teardown);

    g_test_add ("/deserialize/twice_on_same_data", Fixture, NULL, setup, deserialize_twice_on_same_data, teardown);
    g_test_add ("/deserialize/invalid_utf8_data", Fixture, NULL, setup, deserialize_invalid_utf8_data, teardown);
    g_test_add ("/deserialize/swapped_but_valid_contents", Fixture, NULL, setup, deserialize_swapped_but_valid_contents, teardown);

    return g_test_run();
}

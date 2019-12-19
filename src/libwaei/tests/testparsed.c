#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "parsed.h"
#include "parsedline.h"


struct _Fixture {
  LwParsed * parsed;
  gchar * path;
  LwCacheFile * cache_file;
  LwParsedLine * lines;
  GArray * lines_copy;
  GArray * lines_half_copy;
  gboolean lines_copy_cancelled;
  LwProgress * progress;
  GArray * steps;
};
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
    if (lw_progress_get_fraction (self) > .3)
    {
      lw_progress_cancel (self);
    }
}


static gchar const * CONTENTS = "語彙\0ごい\0vocabulary\0日本語\0にほんご\0Japanese";

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->steps = g_array_sized_new (FALSE, TRUE, sizeof(gint), 20);
    fixture->path = g_build_filename (g_get_tmp_dir(), "testparse.XXXXXX", NULL);
    fixture->cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_write (fixture->cache_file, "test checksum", CONTENTS, G_N_ELEMENTS (CONTENTS), NULL);
    lw_cachefile_read (fixture->cache_file, "test checksum", NULL);
    fixture->parsed = lw_parsed_new (fixture->cache_file);

    fixture->progress = lw_progress_new ();
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (append_progress), fixture);
    lw_progress_set_prefered_chunk_size (fixture->progress, 2);
    fixture->steps = g_array_sized_new (FALSE, TRUE, sizeof(gint), 20);

    LwParsedLine * lines = NULL;

    fixture->lines_copy = g_array_sized_new (FALSE, TRUE, sizeof(LwParsedLine), 2);
    fixture->lines_half_copy = g_array_sized_new (FALSE, TRUE, sizeof(LwParsedLine), 2);

    fixture->lines = lines = g_new0 (LwParsedLine, 2);
    lw_parsedline_init_full (&lines[0], (GDestroyNotify) g_free);
    {
      gchar const ** str = g_new0 (gchar const*, 2);
      str[0] = CONTENTS;
      lw_parsedline_set_strv (lines, 1, (gchar**) str);
    }
    {
      gchar const ** str = g_new0 (gchar const*, 2);
      str[0] = CONTENTS + strlen("語彙 ");
      lw_parsedline_set_strv (lines, 2, (gchar**) str);
    }
    {
      gchar const ** str = g_new0 (gchar const*, 2);
      str[0] = CONTENTS + strlen("語彙 ごい ");
      lw_parsedline_set_strv (lines, 3, (gchar **)str);
    }
    lw_parsedline_init_full (&lines[1], (GDestroyNotify) g_free);
    {
      gchar const ** str = g_new0 (gchar const*, 4);
      str[0] = CONTENTS + strlen("語彙 ごい vocabulary ");
      lw_parsedline_set_strv (lines + 1, 1, (gchar**) str);
    }
    {
      gchar const ** str = g_new0 (gchar const*, 2);
      str[0] = CONTENTS + strlen("語彙 ごい vocabulary 日本語 ");
      lw_parsedline_set_strv (lines + 1, 2, (gchar**) str);
    }
    {
      gchar const ** str = g_new0 (gchar const*, 2);
      str[0] = CONTENTS + strlen("語彙 ごい vocabulary 日本語 にほんご ");
      lw_parsedline_set_strv (lines + 1, 3, (gchar**) str);
    }
}


static gboolean
_copy_all_lines (LwParsed * self, LwParsedLine * line, Fixture * fixture)
{
    g_array_append_val (fixture->lines_copy, line);
    return FALSE;
}

static gboolean
_copy_half_lines (LwParsed * self, LwParsedLine * line, Fixture * fixture)
{
    g_assert_false (fixture->lines_copy_cancelled);
    fixture->lines_copy_cancelled = TRUE;
    if (fixture->lines_half_copy->len > 0) return TRUE;
    g_array_append_val (fixture->lines_copy, line);
}


void teardown (Fixture *fixture, gconstpointer data)
{
  gint num_lines = lw_parsed_num_lines (fixture->parsed);
  gint i = 0;

  g_remove (fixture->path);
  g_free (fixture->path);
  g_object_unref (fixture->cache_file);
  g_object_unref (fixture->parsed);

  g_array_free (fixture->lines_copy, TRUE);
  fixture->lines_copy = NULL;

  g_array_free (fixture->lines_half_copy, TRUE);
  fixture->lines_half_copy = NULL;

  if (fixture->lines != NULL)
  {
    while (i < num_lines)
    {
      lw_parsedline_clear (fixture->lines + i);
      i++;
    }
    g_free (fixture->lines);
    fixture->lines = NULL;
  }

  g_array_free (fixture->steps, TRUE);
  fixture->steps = NULL;
}


void
set_lines (Fixture *fixture, gconstpointer data)
{
  //Act
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  //Assert
  g_assert_cmpint (2, ==, lw_parsed_num_lines (fixture->parsed));
  g_assert (fixture->lines + 0 == lw_parsed_get_line (fixture->parsed, 0));
  g_assert (fixture->lines + 1 == lw_parsed_get_line (fixture->parsed, 1));

  g_assert (lw_parsed_get_cachefile (fixture->parsed) == fixture->cache_file);
  fixture->lines = NULL;
}


void
foreach_copy_all_lines (Fixture *fixture, gconstpointer data)
{
  //Act
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  //Assert
  lw_parsed_foreach (fixture->parsed, (LwParsedForeachFunc) _copy_all_lines, fixture);
  g_assert_cmpint (2, ==, fixture->lines_copy->len);
  g_assert (fixture->lines + 0 == g_array_index (fixture->lines_copy, LwParsedLine*, 0));
  g_assert (fixture->lines + 1 == g_array_index (fixture->lines_copy, LwParsedLine*, 1));

  fixture->lines = NULL;
}


void
foreach_copy_half_lines (Fixture *fixture, gconstpointer data)
{
  //Act
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  //Assert
  lw_parsed_foreach (fixture->parsed, (LwParsedForeachFunc) _copy_half_lines, fixture);
  g_assert_cmpint (1, ==, fixture->lines_copy->len);
  g_assert (fixture->lines + 0 == g_array_index (fixture->lines_copy, LwParsedLine*, 0));
  g_assert_true (fixture->lines_copy_cancelled);

  fixture->lines = NULL;
}


void
get_serialized_length (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, progress);

  g_assert_cmpint (length, ==, 160);
  
  gint i = 0;
  gint expected_steps[] = { 0, 33, 100};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }
  g_assert_no_error (lw_progress_get_error (progress));

  fixture->lines = NULL;
}


void
get_serialized_length_with_no_progress (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = NULL;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, progress);

  g_assert_cmpint (length, ==, 160);
  
  gint i = 0;
  gint expected_steps[] = {};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }

  fixture->lines = NULL;
}


void
get_serialized_length_where_cancelled_halfway (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  lw_progress_set_prefered_chunk_size (fixture->progress, 1);
  g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);
  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, progress);

  g_assert_cmpint (length, ==, 8);
  
  gint i = 0;
  gint expected_steps[] = {0, 33};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }

  fixture->lines = NULL;
}


void
get_serialized_length_with_no_lines (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, NULL, 0);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, progress);

  g_assert_cmpint (length, ==, 8);
  
  gint i = 0;
  gint expected_steps[] = {0, 100};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }
  g_assert_no_error (lw_progress_get_error (progress));

  fixture->lines = NULL;
}


void
serialize (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, progress);

  g_assert_cmpint (length, ==, 160);
  g_assert_cmpint (bytes_written, ==, 160);
  
  gint i = 0;
  gint expected_steps[] = { 0, 33, 100};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }
  g_assert_no_error (lw_progress_get_error (progress));

  fixture->lines = NULL;
  g_free (preallocated_buffer);
}


void
serialize_with_no_progress (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = NULL;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, progress);

  g_assert_cmpint (length, ==, 160);
  g_assert_cmpint (bytes_written, ==, 160);
  
  gint i = 0;
  gint expected_steps[] = {};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }

  fixture->lines = NULL;
  g_free (preallocated_buffer);
}


void
serialize_where_cancelled_halfway (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  lw_progress_set_prefered_chunk_size (fixture->progress, 1);
  g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);
  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, progress);

  g_assert_cmpint (length, ==, 160);
  g_assert_cmpint (bytes_written, ==, 8);
  
  gint i = 0;
  gint expected_steps[] = {0, 33};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }

  fixture->lines = NULL;
  g_free (preallocated_buffer);
}


void
serialize_with_no_lines (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, NULL, 0);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, progress);

  g_assert_cmpint (length, ==, 8);
  g_assert_cmpint (bytes_written, ==, 8);
  
  gint i = 0;
  gint expected_steps[] = {0, 100};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }
  g_assert_no_error (lw_progress_get_error (progress));

  fixture->lines = NULL;
  g_free (preallocated_buffer);
}


void
deserialize_with_valid_data (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, NULL);
  bytes_read = lw_serializable_deserialize_into (LW_SERIALIZABLE(parsed_copy), preallocated_buffer, length, progress);

  g_assert_cmpuint (length, ==, bytes_written);
  g_assert_cmpuint (length, ==, bytes_read);

  g_assert_cmpuint (lw_parsed_num_lines (fixture->parsed), ==, lw_parsed_num_lines (parsed_copy));

  gint i = 0;
  gint expected_steps[] = { 0, 5, 52, 100};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }

  {
    LwParsedLine * parsed_line = lw_parsed_get_line (parsed_copy, 0);
    {
      gchar const ** strv = lw_parsedline_get_strv (parsed_line, 1);
      g_assert_cmpstr (strv[0], ==, "語彙");
      g_assert_cmpstr (strv[1], ==, NULL);
    }

    {
      gchar const ** strv = lw_parsedline_get_strv (parsed_line, 2);
      g_assert_cmpstr (strv[0], ==, "ごい");
      g_assert_cmpstr (strv[1], ==, NULL);
    }

    {
      gchar const ** strv = lw_parsedline_get_strv (parsed_line, 3);
      g_assert_cmpstr (strv[0], ==, "vocabulary");
      g_assert_cmpstr (strv[1], ==, NULL);
    }
  }

  {
    LwParsedLine * parsed_line = lw_parsed_get_line (parsed_copy, 1);
    {
      gchar const ** strv = lw_parsedline_get_strv (parsed_line, 1);
      g_assert_cmpstr (strv[0], ==, "日本語");
      g_assert_cmpstr (strv[1], ==, NULL);
    }

    {
      gchar const ** strv = lw_parsedline_get_strv (parsed_line, 2);
      g_assert_cmpstr (strv[0], ==, "にほんご");
      g_assert_cmpstr (strv[1], ==, NULL);
    }

    {
      gchar const ** strv = lw_parsedline_get_strv (parsed_line, 3);
      g_assert_cmpstr (strv[0], ==, "Japanese");
      g_assert_cmpstr (strv[1], ==, NULL);
    }
  }

  {
    gsize num_lines = 0;
    LwParsedLine * lines = lw_parsed_get_lines (parsed_copy, &num_lines);
    g_assert_nonnull (lines);
    g_assert_cmpuint (num_lines, >=, 0);
  }

  fixture->lines = NULL;

  g_object_unref (parsed_copy);
  parsed_copy = NULL;

  g_free (preallocated_buffer);
  preallocated_buffer = NULL;
}


void
deserialize_has_less_lines_than_stated (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;
  gsize num_lines = 1;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, NULL);
  memcpy(preallocated_buffer, &num_lines, sizeof(gsize));
  bytes_read = lw_serializable_deserialize_into (LW_SERIALIZABLE(parsed_copy), preallocated_buffer, length, progress);

  g_assert_error (lw_progress_get_error (progress), LW_PARSED_ERROR, LW_PARSED_ERRORCODE_DESERIALIZATION_ERROR);
  g_assert_cmpuint (length, ==, bytes_written);
  g_assert_cmpuint (84, ==, bytes_read);

  {
    gsize num_lines = 0;
    LwParsedLine * lines = lw_parsed_get_lines (parsed_copy, &num_lines);
    g_assert_null (lines);
    g_assert_cmpuint (num_lines, ==, 0);
  }

  fixture->lines = NULL;

  g_object_unref (parsed_copy);
  parsed_copy = NULL;

  g_free (preallocated_buffer);
  preallocated_buffer = NULL;
}


void
deserialize_has_more_lines_than_stated (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;
  gsize num_lines = 4;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, NULL);
  memcpy(preallocated_buffer, &num_lines, sizeof(gsize));
  bytes_read = lw_serializable_deserialize_into (LW_SERIALIZABLE(parsed_copy), preallocated_buffer, length, progress);

  g_assert_error (lw_progress_get_error (progress), LW_PARSED_ERROR, LW_PARSED_ERRORCODE_DESERIALIZATION_ERROR);
  g_assert_cmpuint (length, ==, bytes_written);
  g_assert_cmpuint (168, ==, bytes_read);

  {
    gsize num_lines = 0;
    LwParsedLine * lines = lw_parsed_get_lines (parsed_copy, &num_lines);
    g_assert_null (lines);
    g_assert_cmpuint (num_lines, ==, 0);
  }

  fixture->lines = NULL;

  g_object_unref (parsed_copy);
  parsed_copy = NULL;

  g_free (preallocated_buffer);
  preallocated_buffer = NULL;
}


void
deserialize_with_no_progress (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = NULL;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, NULL);
  bytes_read = lw_serializable_deserialize_into (LW_SERIALIZABLE(parsed_copy), preallocated_buffer, length, progress);

  g_assert_cmpint (length, ==, 160);
  g_assert_cmpint (bytes_read, ==, 160);
  
  gint i = 0;
  gint expected_steps[] = {};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }

  {
    gsize num_lines = 0;
    LwParsedLine * lines = lw_parsed_get_lines (parsed_copy, &num_lines);
    g_assert_nonnull (lines);
    g_assert_cmpuint (num_lines, >=, 0);
  }

  fixture->lines = NULL;
  g_free (preallocated_buffer);

  g_object_unref (parsed_copy);
  parsed_copy = NULL;
}


void
deserialize_where_cancelled_halfway (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  lw_progress_set_prefered_chunk_size (fixture->progress, 1);
  g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);
  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, NULL);
  bytes_read = lw_serializable_deserialize_into (LW_SERIALIZABLE(parsed_copy), preallocated_buffer, length, progress);

  g_assert_cmpint (length, ==, 160);
  g_assert_cmpint (bytes_written, ==, 160);
  g_assert_cmpint (bytes_read, ==, 84);
  
  gint i = 0;
  gint expected_steps[] = {0, 5, 52};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }

  {
    gsize num_lines = 0;
    LwParsedLine * lines = lw_parsed_get_lines (parsed_copy, &num_lines);
    g_assert_null (lines);
    g_assert_cmpuint (num_lines, ==, 0);
  }

  fixture->lines = NULL;
  g_free (preallocated_buffer);

  g_object_unref (parsed_copy);
  parsed_copy = NULL;
}


void
deserialize_with_no_lines (Fixture * fixture, gconstpointer data)
{
  lw_parsed_set_lines (fixture->parsed, NULL, 0);

  gsize length = 0;
  gsize bytes_written = 0;
  gsize bytes_read = 0;
  LwParsed * parsed_copy = NULL;
  LwProgress * progress = fixture->progress;
  LwSerializable * serializable = NULL;
  gchar * preallocated_buffer = NULL;

  parsed_copy = lw_parsed_new (fixture->cache_file);
  serializable = LW_SERIALIZABLE (fixture->parsed);
  length = lw_serializable_get_serialized_length (serializable, NULL);
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, NULL);
  bytes_read = lw_serializable_deserialize_into (LW_SERIALIZABLE(parsed_copy), preallocated_buffer, length, progress);

  g_assert_cmpint (length, ==, 8);
  g_assert_cmpint (bytes_read, ==, 8);
  
  gint i = 0;
  gint expected_steps[] = {0, 100};
  g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
  for (i = 0; i < fixture->steps->len; i++)
  {
    g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
  }
  g_assert_no_error (lw_progress_get_error (progress));

  fixture->lines = NULL;
  g_free (preallocated_buffer);

  g_object_unref (parsed_copy);
  parsed_copy = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/set_lines", Fixture, NULL, setup, set_lines, teardown);

    g_test_add ("/foreach/copy_all_lines", Fixture, NULL, setup, foreach_copy_all_lines, teardown);
    g_test_add ("/foreach/copy_half_lines", Fixture, NULL, setup, foreach_copy_half_lines, teardown);

    g_test_add ("/get_serialized_length", Fixture, NULL, setup, get_serialized_length, teardown);
    g_test_add ("/get_serialized_length_with_no_progress", Fixture, NULL, setup, get_serialized_length_with_no_progress, teardown);
    g_test_add ("/get_serialized_length_with_no_lines", Fixture, NULL, setup, get_serialized_length_with_no_lines, teardown);
    g_test_add ("/get_serialized_length_where_cancelled_halfway", Fixture, NULL, setup, get_serialized_length_where_cancelled_halfway, teardown);

    g_test_add ("/serialize", Fixture, NULL, setup, serialize, teardown);
    g_test_add ("/serialize_with_no_progress", Fixture, NULL, setup, serialize_with_no_progress, teardown);
    g_test_add ("/serialize_with_no_lines", Fixture, NULL, setup, serialize_with_no_lines, teardown);
    g_test_add ("/serialize_where_cancelled_halfway", Fixture, NULL, setup, serialize_where_cancelled_halfway, teardown);

    g_test_add ("/deserialize/with_valid_data", Fixture, NULL, setup, deserialize_with_valid_data, teardown);
    g_test_add ("/deserialize/has_less_lines_than_stated", Fixture, NULL, setup, deserialize_has_less_lines_than_stated, teardown);
    g_test_add ("/deserialize/has_more_lines_than_stated", Fixture, NULL, setup, deserialize_has_more_lines_than_stated, teardown);
    g_test_add ("/deserialize/with_no_progress", Fixture, NULL, setup, deserialize_with_no_progress, teardown);
    g_test_add ("/deserialize/with_no_lines", Fixture, NULL, setup, deserialize_with_no_lines, teardown);
    g_test_add ("/deserialize/where_cancelled_halfway", Fixture, NULL, setup, deserialize_where_cancelled_halfway, teardown);

    return g_test_run();
}

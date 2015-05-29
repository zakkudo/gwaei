#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/parsed.h>
#include <libwaei/parsedline.h>


struct _Fixture {
  LwParsed * parsed;
  gchar * path;
  LwCacheFile * cache_file;
  LwParsedLine * lines;
  GArray * lines_copy;
  GArray * lines_half_copy;
  gboolean lines_copy_cancelled;
};
typedef struct _Fixture Fixture;


static gchar const * CONTENTS = "語彙\0ごい\0vocabulary\0日本語\0にほんご\0Japanese";

void setup (Fixture *fixture, gconstpointer data)
{
  fixture->path = g_build_filename (g_get_tmp_dir(), "testparse.XXXXXX", NULL);
  fixture->cache_file = lw_cachefile_new (fixture->path);
  lw_cachefile_write (fixture->cache_file, "test checksum", CONTENTS, G_N_ELEMENTS (CONTENTS), NULL);
  lw_cachefile_read (fixture->cache_file, "test checksum", NULL);
  fixture->parsed = lw_parsed_new (fixture->cache_file);

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
deserialize (Fixture * fixture, gconstpointer data)
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
  preallocated_buffer = g_new0 (gchar, length);
  bytes_written = lw_serializable_serialize (serializable, preallocated_buffer, progress);
  bytes_read = lw_serializable_deserialize_into (LW_SERIALIZABLE(parsed_copy), preallocated_buffer, length, progress);

  g_assert_cmpuint (length, ==, bytes_written);
  g_assert_cmpuint (length, ==, bytes_read);

  fixture->lines = NULL;

  g_object_unref (parsed_copy);
  parsed_copy = NULL;

  g_free (preallocated_buffer);
  preallocated_buffer = NULL;
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/set_lines", Fixture, NULL, setup, set_lines, teardown);
    g_test_add ("/foreach/copy_all_lines", Fixture, NULL, setup, foreach_copy_all_lines, teardown);
    g_test_add ("/foreach/copy_half_lines", Fixture, NULL, setup, foreach_copy_half_lines, teardown);

    g_test_add ("/deserialize", Fixture, NULL, setup, deserialize, teardown);

    return g_test_run();
}

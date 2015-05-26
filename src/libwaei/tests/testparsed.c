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
};
typedef struct _Fixture Fixture;


static gchar const * CONTENTS = "語彙\0ごい\0vocabulary\0日本語\0にほんご\0Japanese";

void setup (Fixture *fixture, gconstpointer data)
{
  fixture->path = g_build_filename (g_get_tmp_dir(), "testparse.XXXXXX", NULL);
  fixture->cache_file = lw_cachefile_new (fixture->path);
  lw_cachefile_write (fixture->cache_file, "test checksum", CONTENTS, G_N_ELEMENTS (CONTENTS), NULL);
  lw_cachefile_read(fixture->cache_file, "test checksum", NULL);
  fixture->parsed = lw_parsed_new (fixture->cache_file);

  LwParsedLine * lines = NULL;

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


void teardown (Fixture *fixture, gconstpointer data)
{
  gint num_lines = lw_parsed_num_lines (fixture->parsed);
  gint i = 0;

  g_remove (fixture->path);
  g_free (fixture->path);
  g_object_unref (fixture->cache_file);
  g_object_unref (fixture->parsed);

  while (i < num_lines)
  {
    lw_parsedline_clear (fixture->lines + i);
  }
  g_free (fixture->lines);
}


void
set_lines (Fixture *fixture, gconstpointer data)
{
  //Act
  //lw_parsed_set_lines (fixture->parsed, fixture->lines, 2);

  //Assert
  //g_assert_cmpint (2, ==, lw_parsed_num_lines (fixture->parsed));
  //g_assert (fixture->lines == lw_parsed_get_line (fixture->parsed, 1));
//  lw_parsed_foreach ()
//  lw_parsed_get_cachefile ()
}

gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/set_lines", Fixture, NULL, setup, set_lines, teardown);

    return g_test_run();
}

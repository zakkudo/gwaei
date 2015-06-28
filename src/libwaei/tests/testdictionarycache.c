#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

#include "testdictionary.h"

struct _Fixture {
  gchar * cachetmpl;
  gchar * cachedir;
  LwDictionaryCache * cache;
};
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->cachetmpl = g_build_filename (g_get_tmp_dir (), "testcachedir-XXXXXX", NULL);
    fixture->cachedir = g_mkdtemp_full (fixture->cachetmpl, 0700);
    g_setenv("DICTIONARYCACHEDIR", fixture->cachedir, TRUE);
    
    fixture->cache = lw_dictionarycache_new ("test", LW_TYPE_TESTDICTIONARY, LW_UTF8FLAG_NONE);
}


void teardown (Fixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->cache);

    g_remove (fixture->cachedir);
    g_free (fixture->cachedir);
}


LwParsed *
parse (LwCacheFile * cache_file,
       gpointer      data)
{
    LwParsed * parsed = NULL;
    gchar * contents = NULL;
    gsize content_length = 0;
    gsize num_lines = 0;
    
    parsed = lw_parsed_new (cache_file);

    contents = lw_cachefile_get_contents (cache_file);
    content_length = lw_cachefile_length (cache_file);
    num_lines = lw_utf8_replace_linebreaks_with_nullcharacter (contents, content_length, NULL, NULL);

    {
      LwParsedLine *lines = g_new0 (LwParsedLine, num_lines);
      gint i = 0;
      gchar * c = contents;

      for (i = 0; i < num_lines; i++)
      {
        lw_parsedline_init_full (lines + i, (GDestroyNotify) g_free);
        gchar ** p = g_new0 (gchar*, 2);
        p[0] = c;
        lw_parsedline_set_strv (lines + i, 0, p);

        while (*c != '\0') c++;
        c++;
      }

      lw_parsed_set_lines (parsed, lines, num_lines);
    }

    return parsed;
}

void
test_write (Fixture       * fixture,
            gconstpointer   data)
{
    gchar const * body = "one\ntwo\nthree";
    lw_dictionarycache_write (fixture->cache, "Test Checksum", body, strlen(body) + 1, parse, NULL, NULL);
    lw_dictionarycache_read (fixture->cache, "Test Checksum", NULL);

    LwParsed * parsed = lw_dictionarycache_get_parsed (fixture->cache);
    LwCacheFile * cache_file = lw_parsed_get_cachefile (parsed);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);

    {
      LwParsedLine * line = lw_parsed_get_line (parsed, 0);
      g_assert_cmpstr (lw_parsedline_get_strv (line, 0)[0], ==, "one");
      g_assert_cmpstr (lw_parsedline_get_strv (line, 0)[1], ==, NULL);

      line = lw_parsed_get_line (parsed, 1);
      g_assert_cmpstr (lw_parsedline_get_strv (line, 0)[0], ==, "two");
      g_assert_cmpstr (lw_parsedline_get_strv (line, 0)[1], ==, NULL);

      line = lw_parsed_get_line (parsed, 2);
      g_assert_cmpstr (lw_parsedline_get_strv (line, 0)[0], ==, "three");
      g_assert_cmpstr (lw_parsedline_get_strv (line, 0)[1], ==, NULL);
    }

    g_assert_true (memcmp(contents, "one\0two\0three", sizeof("one\0two\0three")) == 0);
    g_assert_cmpint (length, ==, 14);

    {
      gchar * normalized_contents = g_build_filename (fixture->cachedir, "libwaei", "dictionary", "LwTestDictionary", "test.normalized", NULL);
      gchar * parsed_contents = g_build_filename (fixture->cachedir, "libwaei", "dictionary", "LwTestDictionary", "test.parsed", NULL);

      g_assert_true (g_file_test (normalized_contents, G_FILE_TEST_IS_REGULAR));
      g_assert_true (g_file_test (parsed_contents, G_FILE_TEST_IS_REGULAR));

      g_free (normalized_contents);
      g_free (parsed_contents);
    }
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/test_write", Fixture, NULL, setup, test_write, teardown);

    return g_test_run();
}






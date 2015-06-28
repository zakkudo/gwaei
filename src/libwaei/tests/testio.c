#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

struct _Fixture {
  gchar * path;
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
  if (lw_progress_get_fraction (self) > .5)
  {
    lw_progress_cancel (self);
  }
}


void setup (Fixture *fixture, gconstpointer data)
{
  fixture->path = g_build_filename (g_get_tmp_dir (), "testfile.XXXXXX", NULL);

  fixture->progress = lw_progress_new ();
  g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (append_progress), fixture);
  lw_progress_set_prefered_chunk_size (fixture->progress, 2);
  fixture->steps = g_array_sized_new (FALSE, TRUE, sizeof(gint), 20);

}


void teardown (Fixture *fixture, gconstpointer data)
{
  if (fixture->path != NULL) g_remove (fixture->path);
  g_free (fixture->path);
  g_object_unref (fixture->progress);
  fixture->progress = NULL;
  g_array_free (fixture->steps, TRUE);
}


void
fwrite_chunked_string_with_length (Fixture       * fixture,
                                   gconstpointer   data)
{
    //Arrange
    gint fd = g_mkstemp(fixture->path);
    close(fd);

    LwProgress * progress = fixture->progress;
    gchar const * TEXT = "test content";
    gsize length = 0;
    FILE * stream = NULL;
    gchar * contents = NULL;
    gint i = 0;

    stream = g_fopen(fixture->path, "wb");

    //Act
    lw_io_fwrite_chunked (stream, TEXT, 13, progress);
    fclose(stream);

    //Assert
    gint expected_steps[] = { 0, 15, 30, 46, 61, 76, 92, 100};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_file_get_contents (fixture->path, &contents, &length, NULL);
    g_assert_cmpstr (contents, ==, "test content");
    g_assert_cmpuint (length, ==, 13);

    g_free (contents);
}


void
fwrite_chunked_string_with_no_length (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Arrange
    gint fd = g_mkstemp(fixture->path);
    close(fd);

    LwProgress * progress = fixture->progress;
    gchar const * TEXT = "test content";
    gsize length = 0;
    FILE * stream = NULL;
    gchar * contents = NULL;
    gint i = 0;

    stream = g_fopen(fixture->path, "wb");

    //Act
    lw_io_fwrite_chunked (stream, TEXT, 0, progress);
    fclose(stream);

    //Assert
    gint expected_steps[] = {};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_file_get_contents (fixture->path, &contents, &length, NULL);
    g_assert_cmpstr (contents, ==, "");
    g_assert_cmpuint (length, ==, 0);

    g_free (contents);
}


void
fwrite_chunked_string_with_no_progress (Fixture       * fixture,
                                        gconstpointer   data)
{
    //Arrange
    gint fd = g_mkstemp(fixture->path);
    close(fd);

    LwProgress * progress = fixture->progress;
    gchar const * TEXT = "test content";
    gsize length = 0;
    FILE * stream = NULL;
    gchar * contents = NULL;
    gint i = 0;

    stream = g_fopen(fixture->path, "wb");

    //Act
    lw_io_fwrite_chunked (stream, TEXT, 13, NULL);
    fclose(stream);

    //Assert
    g_file_get_contents (fixture->path, &contents, &length, NULL);
    g_assert_cmpstr (contents, ==, "test content");
    g_assert_cmpuint (length, ==, 13);

    g_free (contents);
}


void
fwrite_chunked_string_where_is_cancelled (Fixture       * fixture,
                                          gconstpointer   data)
{
    //Arrange
    gint fd = g_mkstemp(fixture->path);
    close(fd);

    LwProgress * progress = fixture->progress;
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);

    gchar const * TEXT = "test content";
    gsize length = 0;
    FILE * stream = NULL;
    gchar * contents = NULL;
    gint i = 0;

    stream = g_fopen(fixture->path, "wb");

    //Act
    lw_io_fwrite_chunked (stream, TEXT, 13, progress);
    fclose(stream);

    //Assert
    gint expected_steps[] = { 0, 15, 30, 46, 61};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_file_get_contents (fixture->path, &contents, &length, NULL);
    g_assert_cmpstr (contents, ==, "test conte");
    g_assert_cmpuint (length, ==, 10);

    g_free (contents);
}


void
get_pagesize (Fixture       * fixture,
              gconstpointer   data)
{
    g_assert_cmpuint (lw_io_get_pagesize (), >, 0);
}


void
allocate_temporary_file (Fixture       * fixture,
                         gconstpointer   data)
{
    // Arrange
    LwProgress * progress = fixture->progress;
    gint i = 0;

    // Act
    gchar * filename = lw_io_allocate_temporary_file (15, progress);

    // Assert
    gint expected_steps[] = { 0, 13, 26, 40, 53, 66, 80, 93, 100 };
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_assert_true (g_file_test (filename, G_FILE_TEST_IS_REGULAR));
    gchar * buffer = g_new0 (gchar, 15);
    gsize length = 0;
    gchar * contents = NULL;
    g_file_get_contents (filename, &contents, &length, NULL);
    g_assert_true (memcmp(buffer, contents, sizeof(gchar) * 15) == 0);

    g_free (buffer);
    g_free (contents);
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/fwrite_chunked/string_with_length", Fixture, NULL, setup, fwrite_chunked_string_with_length, teardown);
    g_test_add ("/fwrite_chunked/string_with_no_length", Fixture, NULL, setup, fwrite_chunked_string_with_no_length, teardown);
    g_test_add ("/fwrite_chunked/string_with_no_progress", Fixture, NULL, setup, fwrite_chunked_string_with_no_progress, teardown);
    g_test_add ("/fwrite_chunked/string_where_is_cancelled", Fixture, NULL, setup, fwrite_chunked_string_where_is_cancelled, teardown);

    g_test_add ("/get_pagesize", Fixture, NULL, setup, get_pagesize, teardown);

    g_test_add ("/allocate_temporary_file", Fixture, NULL, setup, allocate_temporary_file, teardown);

    return g_test_run();
}





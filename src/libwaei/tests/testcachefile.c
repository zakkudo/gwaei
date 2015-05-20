#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <libwaei/cachefile.h>


struct _Fixture { gchar * path; gchar * path2; LwProgress * progress; GArray *steps;};
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
    fixture->path = g_build_filename (g_get_tmp_dir (), "testcachefile.XXXXXX", NULL);
    fixture->path2 = g_build_filename (g_get_tmp_dir (), "testcachefile.XXXXXX", NULL);

    fixture->progress = lw_progress_new ();
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (append_progress), fixture);
    lw_progress_set_prefered_chunk_size (fixture->progress, 2);
    fixture->steps = g_array_sized_new (FALSE, TRUE, sizeof(gint), 20);
}


void teardown (Fixture *fixture, gconstpointer data)
{
    g_remove (fixture->path);
    g_free (fixture->path);
    g_remove (fixture->path2);
    g_free (fixture->path2);

    g_object_unref (fixture->progress);
    fixture->progress = NULL;
    g_array_free (fixture->steps, TRUE);
    fixture->steps = NULL;
}




void
new_with_existing_file (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    gint i = 0;
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test checksum", strlen("test checksum") + 1);
    write(fd, "test content", strlen("test content") + 1);
    close(fd);

    //Act
    LwCacheFile * cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_read (cache_file, "test checksum", progress);

    //Assert
    gint expected_steps[] = { 0, 15, 38, 61, 84, 100};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }

    g_assert_false (lw_progress_completed (fixture->progress));
    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, "test checksum");
    g_assert_cmpstr (contents, ==, "test content");
    g_assert_cmpuint (length, ==, strlen("test content") + 1);

    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
read_with_invalid_checksum (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    gint i = 0;
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test checksum", strlen("test checksum") + 1);
    write(fd, "test content", strlen("test content") + 1);
    close(fd);

    //Act
    LwCacheFile * cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_read (cache_file, "test invalid checksum", progress);

    //Assert
    gint expected_steps[] = { 0, 15, 38, 61, 84, 100};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, NULL);
    g_assert_cmpstr (contents, ==, NULL);
    g_assert_cmpuint (length, ==, 0);

    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
read_where_cancels_halfway (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);
    gint i = 0;
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test checksum", strlen("test checksum") + 1);
    write(fd, "test content", strlen("test content") + 1);
    close(fd);

    //Act
    LwCacheFile * cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_read (cache_file, "test invalid checksum", progress);

    //Assert
    gint expected_steps[] = { 0, 15, 38, 61};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, NULL);
    g_assert_cmpstr (contents, ==, NULL);
    g_assert_cmpuint (length, ==, 0);

    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
read_with_no_progress (Fixture *fixture, gconstpointer data)
{
    //Arrange
    gint i = 0;
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test checksum", strlen("test checksum") + 1);
    write(fd, "test content", strlen("test content") + 1);
    close(fd);

    //Act
    LwCacheFile * cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_read (cache_file, "test invalid checksum", NULL);

    //Assert
    gint expected_steps[] = {};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, NULL);
    g_assert_cmpstr (contents, ==, NULL);
    g_assert_cmpuint (length, ==, 0);

    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
write_with_string (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    gint i = 0;
    gint fd = g_mkstemp(fixture->path);
    close(fd);

    //Act
    LwCacheFile * cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_write (cache_file, "test checksum", "test content", strlen("test content") + 1, progress);
    lw_cachefile_read (cache_file, "test checksum", progress);

    //Assert
    gint expected_steps[] = { 0, 15, 30, 46, 61, 76, 92, 100, 0, 15, 38, 61, 84, 100};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, "test checksum");
    g_assert_cmpstr (contents, ==, "test content");
    g_assert_cmpuint (length, ==, strlen("test content") + 1);

    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
write_where_cancels_halfway (Fixture *fixture, gconstpointer data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);
    gint i = 0;
    gint fd = g_mkstemp(fixture->path);
    close(fd);

    //Act
    LwCacheFile * cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_write (cache_file, "test checksum", "test content", strlen("test content") + 1, progress);
    lw_cachefile_read (cache_file, "test checksum", NULL);

    //Assert
    gint expected_steps[] = { 0, 15, 30, 46, 61};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, "test checksum");
    g_assert_cmpstr (contents, ==, "test conte");
    g_assert_cmpuint (length, ==, strlen("test conte"));

    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
write_with_no_progress (Fixture *fixture, gconstpointer data)
{
    //Arrange
    gint i = 0;
    gint fd = g_mkstemp(fixture->path);
    close(fd);

    //Act
    LwCacheFile * cache_file = lw_cachefile_new (fixture->path);
    lw_cachefile_write (cache_file, "test checksum", "test content", strlen("test content") + 1, NULL);
    lw_cachefile_read (cache_file, "test checksum", NULL);

    //Assert
    gint expected_steps[] = { };
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, "test checksum");
    g_assert_cmpstr (contents, ==, "test content");
    g_assert_cmpuint (length, ==, strlen("test content") + 1);

    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
write_cachefile (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test checksum", strlen("test checksum") + 1);
    write(fd, "test content", strlen("test content") + 1);
    close(fd);

    LwCacheFile * cache_file_original = lw_cachefile_new (fixture->path);
    lw_cachefile_read (cache_file_original, "test checksum", NULL);


    LwCacheFile * cache_file = lw_cachefile_new (fixture->path2);
    lw_cachefile_write_cachefile (cache_file, "test checksum copy", cache_file_original, NULL);
    lw_cachefile_read (cache_file, "test checksum copy", NULL);

    gchar const * CHECKSUM = lw_cachefile_get_checksum (cache_file);
    gchar * contents = lw_cachefile_get_contents (cache_file);
    gsize length = lw_cachefile_length (cache_file);
    g_assert_cmpstr (CHECKSUM, ==, "test checksum copy");
    g_assert_cmpstr (contents, ==, "test content");
    g_assert_cmpuint (length, ==, strlen("test content") + 1);

    g_object_unref (cache_file_original);
    cache_file_original = NULL;
    g_object_unref (cache_file);
    cache_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}



gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/new/with_existing_file", Fixture, NULL, setup, new_with_existing_file, teardown);

    g_test_add ("/read/with_invalid_checksum", Fixture, NULL, setup, read_with_invalid_checksum, teardown);
    g_test_add ("/read/with_no_progress", Fixture, NULL, setup, read_with_no_progress, teardown);
    g_test_add ("/read/where_cancels_halfway", Fixture, NULL, setup, read_where_cancels_halfway, teardown);

    g_test_add ("/write/with_string", Fixture, NULL, setup, write_with_string, teardown);
    g_test_add ("/write/where_cancels_halfway", Fixture, NULL, setup, write_where_cancels_halfway, teardown);
    g_test_add ("/write/with_no_progress", Fixture, NULL, setup, write_with_no_progress, teardown);

    g_test_add ("/write_cachefile", Fixture, NULL, setup, write_cachefile, teardown);

    return g_test_run();
}





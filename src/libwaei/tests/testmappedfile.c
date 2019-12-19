#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include "mappedfile.h"


struct _Fixture { gchar * tmpl; gchar const * TMP_DIR; gchar * path; gchar *tmpl2; gchar *path2;};
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->tmpl = g_strdup("testmappedfile.XXXXXX");
    fixture->tmpl2 = g_strdup("testmappedfile.XXXXXX");
    fixture->TMP_DIR = g_get_tmp_dir ();
    fixture->path = g_build_filename (fixture->TMP_DIR, fixture->tmpl, NULL);
    fixture->path2 = g_build_filename (fixture->TMP_DIR, fixture->tmpl, NULL);
}


void teardown (Fixture *fixture, gconstpointer data)
{
    g_free (fixture->tmpl);
    g_free (fixture->tmpl2);
    g_remove (fixture->path);
    g_remove (fixture->path2);
    g_free (fixture->path);
    g_free (fixture->path2);
}




void
new_with_existing_file (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test contents", strlen("test contents") + 1); 
    close(fd);


    LwMappedFile * mapped_file = lw_mappedfile_new (fixture->path);
    gchar * contents = lw_mappedfile_get_contents (mapped_file);
    g_assert_cmpstr (contents, ==, "test contents");
    g_assert_cmpuint (lw_mappedfile_length (mapped_file), ==, strlen("test contents") + 1);
    g_assert_cmpstr (lw_mappedfile_get_path (mapped_file), ==, fixture->path);

    g_assert_false (lw_mappedfile_get_delete_on_free (mapped_file));
    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));

    g_object_unref (mapped_file);
    mapped_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}

void
new_with_non_existing_file (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test contents", strlen("test contents") + 1); 
    close(fd);

    g_remove (fixture->path);

    LwMappedFile * mapped_file = lw_mappedfile_new (fixture->path);
    gchar * contents = lw_mappedfile_get_contents (mapped_file);
    g_assert_cmpstr (contents, ==, NULL);
    g_assert_cmpuint (lw_mappedfile_length (mapped_file), ==, 0);
    g_assert_cmpstr (lw_mappedfile_get_path (mapped_file), ==, fixture->path);

    g_assert_false (lw_mappedfile_get_delete_on_free (mapped_file));
    g_assert_false (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));

    g_object_unref (mapped_file);
    mapped_file = NULL;

    g_assert_false (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}

void
set_delete_on_free_to_true (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test contents", strlen("test contents") + 1); 
    close(fd);


    LwMappedFile * mapped_file = lw_mappedfile_new (fixture->path);
    lw_mappedfile_set_delete_on_free (mapped_file, TRUE);
    gchar * contents = lw_mappedfile_get_contents (mapped_file);
    g_assert_cmpstr (contents, ==, "test contents");
    g_assert_cmpuint (lw_mappedfile_length (mapped_file), ==, strlen("test contents") + 1);
    g_assert_cmpstr (lw_mappedfile_get_path (mapped_file), ==, fixture->path);

    g_assert_true (lw_mappedfile_get_delete_on_free (mapped_file));
    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));

    g_object_unref (mapped_file);
    mapped_file = NULL;

    g_assert_true (!g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
set_delete_on_free_to_false (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test contents", strlen("test contents") + 1); 
    close(fd);


    LwMappedFile * mapped_file = lw_mappedfile_new (fixture->path);
    gchar * contents = lw_mappedfile_get_contents (mapped_file);
    lw_mappedfile_set_delete_on_free (mapped_file, FALSE);
    g_assert_cmpstr (contents, ==, "test contents");
    g_assert_cmpuint (lw_mappedfile_length (mapped_file), ==, strlen("test contents") + 1);
    g_assert_false (lw_mappedfile_get_delete_on_free (mapped_file));
    g_assert_cmpstr (lw_mappedfile_get_path (mapped_file), ==, fixture->path);

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));

    g_object_unref (mapped_file);
    mapped_file = NULL;

    g_assert_true (g_file_test (fixture->path, G_FILE_TEST_IS_REGULAR));
}


void
is_writable (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test contents", strlen("test contents") + 1); 
    close(fd);


    LwMappedFile * mapped_file = lw_mappedfile_new (fixture->path);
    gchar * contents = lw_mappedfile_get_contents (mapped_file);
    g_assert_cmpstr (contents, ==, "test contents");
    g_assert_cmpuint (lw_mappedfile_length (mapped_file), ==, strlen("test contents") + 1);
    g_assert_cmpstr (lw_mappedfile_get_path (mapped_file), ==, fixture->path);
    g_assert_false (lw_mappedfile_is_writable (mapped_file));
    g_assert_false (lw_mappedfile_get_delete_on_free (mapped_file));

}


void
set_path_to_invalid_file (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test contents", strlen("test contents") + 1); 
    close(fd);

    gint fd2 = g_mkstemp(fixture->path2);
    close(fd);
    g_remove (fixture->path2);

    LwMappedFile * mapped_file = lw_mappedfile_new (fixture->path);
    lw_mappedfile_set_path (mapped_file, fixture->path2);

    gchar * contents = lw_mappedfile_get_contents (mapped_file);

    g_assert_cmpstr (contents, ==, NULL);
    g_assert_cmpuint (lw_mappedfile_length (mapped_file), ==, 0);
    g_assert_cmpstr (lw_mappedfile_get_path (mapped_file), ==, fixture->path2);
    g_assert_false (lw_mappedfile_is_writable (mapped_file));
    g_assert_false (lw_mappedfile_get_delete_on_free (mapped_file));

}


void
set_path_to_other_file (Fixture *fixture, gconstpointer data)
{
    gint fd = g_mkstemp(fixture->path);
    write(fd, "test contents", strlen("test contents") + 1); 
    close(fd);

    gint fd2 = g_mkstemp(fixture->path2);
    write(fd, "test other contents", strlen("test other contents") + 1); 
    close(fd);

    LwMappedFile * mapped_file = lw_mappedfile_new (fixture->path);
    lw_mappedfile_set_path (mapped_file, fixture->path2);

    gchar * contents = lw_mappedfile_get_contents (mapped_file);

    g_assert_cmpstr (contents, ==, "test other contents");
    g_assert_cmpuint (lw_mappedfile_length (mapped_file), ==, 20);
    g_assert_cmpstr (lw_mappedfile_get_path (mapped_file), ==, fixture->path2);
    g_assert_false (lw_mappedfile_is_writable (mapped_file));
    g_assert_false (lw_mappedfile_get_delete_on_free (mapped_file));
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/new/with_existing_file", Fixture, NULL, setup, new_with_existing_file, teardown);
    g_test_add ("/new/with_non_existing_file", Fixture, NULL, setup, new_with_non_existing_file, teardown);

    g_test_add ("/set_delete_on_free/to_true", Fixture, NULL, setup, set_delete_on_free_to_true, teardown);
    g_test_add ("/set_delete_on_free/to_false", Fixture, NULL, setup, set_delete_on_free_to_false, teardown);

    g_test_add ("/is_writable", Fixture, NULL, setup, is_writable, teardown);

    g_test_add ("/set_path/to_invalid_file", Fixture, NULL, setup, set_path_to_invalid_file, teardown);
    g_test_add ("/set_path/to_other_file", Fixture, NULL, setup, set_path_to_other_file, teardown);

    return g_test_run();
}




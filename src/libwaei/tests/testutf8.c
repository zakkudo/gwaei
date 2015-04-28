#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/utf8.h>


struct _Fixture { LwProgress * progress; };
typedef struct _Fixture Fixture;

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->progress = lw_progress_new ();
    lw_progress_set_prefered_chunk_size (fixture->progress, 2);
}
void teardown (Fixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->progress);
    fixture->progress = NULL;
}


void
normalize_japanese_and_english (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "test English search テスト日本語けんさく";
    GError * error = NULL;
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, -1, LW_UTF8FLAG_NONE);

    //Assert
    g_assert_cmpstr ("test English search テスト日本語けんさく", ==, normalized);

    g_free (normalized);
    normalized = NULL;
}

void
normalize_casefold_japanese_and_english (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "test English search テスト日本語けんさく";
    GError * error = NULL;
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, -1, LW_UTF8FLAG_CASEFOLD);

    //Assert
    g_assert_cmpstr ("test english search テスト日本語けんさく", ==, normalized);

    g_free (normalized);
    normalized = NULL;
}

void
normalize_furiganafold_japanese_and_english (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "test English search テスト日本語けんさく";
    GError * error = NULL;
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, -1, LW_UTF8FLAG_FURIGANAFOLD);

    //Assert
    g_assert_cmpstr ("test English search てすと日本語けんさく", ==, normalized);

    g_free (normalized);
    normalized = NULL;
}


void
normalize_allfold_japanese_and_english (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "test English search テスト日本語けんさく";
    GError * error = NULL;
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, -1, LW_UTF8FLAG_ALL);

    //Assert
    g_assert_cmpstr ("test english search てすと日本語けんさく", ==, normalized);

    g_free (normalized);
    normalized = NULL;
}


void
normalize_no_text (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "";
    GError * error = NULL;
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, -1, LW_UTF8FLAG_NONE);

    //Assert
    g_assert_cmpstr ("", ==, normalized);

    g_free (normalized);
    normalized = NULL;
}


void
furiganafold_no_text (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("");
    GError * error = NULL;

    //Initializations
    lw_utf8_furiganafold (text, -1, fixture->progress);

    //Assert
    g_assert_cmpstr ("", ==, text);

    g_free (text);
    text = NULL;
}



gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/furiganafold/no_text", Fixture, NULL, setup, furiganafold_no_text, teardown);

    g_test_add ("/normalize/japanese_and_english", Fixture, NULL, setup, normalize_japanese_and_english, teardown);
    g_test_add ("/normalize/casefold_japanese_and_english", Fixture, NULL, setup, normalize_casefold_japanese_and_english, teardown);
    g_test_add ("/normalize/furiganafold_japanese_and_english", Fixture, NULL, setup, normalize_furiganafold_japanese_and_english, teardown);
    g_test_add ("/normalize/allfold_japanese_and_english", Fixture, NULL, setup, normalize_allfold_japanese_and_english, teardown);
    g_test_add ("/normalize/no_text", Fixture, NULL, setup, normalize_no_text, teardown);

    return g_test_run();
}



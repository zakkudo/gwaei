#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/utf8.h>


struct _Fixture { LwProgress * progress; GArray *steps;};
typedef struct _Fixture Fixture;

static void
append_progress (LwProgress * self, Fixture * fixture)
{
  gint percent = (gint) (lw_progress_get_fraction (self) * 100.0);
  g_array_append_val (fixture->steps, percent);
}

void setup (Fixture *fixture, gconstpointer data)
{
    fixture->progress = lw_progress_new ();
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (append_progress), fixture);
    lw_progress_set_prefered_chunk_size (fixture->progress, 2);
    fixture->steps = g_array_sized_new (FALSE, TRUE, sizeof(gint), 20);
}

void teardown (Fixture *fixture, gconstpointer data)
{
    g_object_unref (fixture->progress);
    fixture->progress = NULL;
    g_array_free (fixture->steps, TRUE);
    fixture->steps = NULL;
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
    gint i = 0;

    //Initializations
    lw_utf8_furiganafold (text, -1, fixture->progress);

    //Assert
    gint expected_steps[] = {100};
    g_assert_cmpstr ("", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }

    g_free (text);
    text = NULL;
}


void
furiganafold_with_mixed_text (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("abcDEFGあいうえおカキクケコ日本語");
    GError * error = NULL;
    gint i = 0;

    //Initializations
    lw_utf8_furiganafold (text, -1, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 0, 8, 21, 47, 73, 100, 100};
    g_assert_cmpstr ("abcDEFGあいうえおかきくけこ日本語", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_true (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}


void
furiganafold_with_all_katakana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶヷヸヹヺ・ーヽヾヿ");
    GError * error = NULL;
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 10);
    lw_utf8_furiganafold (text, -1, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 0, 12, 25, 37, 50, 63, 75, 88, 100, 100};
    g_assert_cmpstr ("ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖヷヸヹヺ・ーゝゞゟ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_true (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}

void
furiganafold_with_all_halfwidth_katakana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("･ ｦ ｧ ｨ ｩ ｪ ｫ ｬ ｭ ｮ ｯｰ ｱ ｲ ｳ ｴ ｵ ｶ ｷ ｸ ｹ ｺ ｻ ｼ ｽ ｾ ｿﾀ ﾁ ﾂ ﾃ ﾄ ﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾋ ﾌ ﾍ ﾎ ﾏﾐ ﾑ ﾒ ﾓ ﾔ ﾕ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ ﾜ ﾝ ﾞ ﾟ");
    GError * error = NULL;
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 10);
    lw_utf8_furiganafold (text, -1, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 0, 10, 21, 31, 42, 52, 62, 74, 84, 94, 100, 100};
    g_assert_cmpstr ("･ ｦ ｧ ｨ ｩ ｪ ｫ ｬ ｭ ｮ ｯｰ ｱ ｲ ｳ ｴ ｵ ｶ ｷ ｸ ｹ ｺ ｻ ｼ ｽ ｾ ｿﾀ ﾁ ﾂ ﾃ ﾄ ﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾋ ﾌ ﾍ ﾎ ﾏﾐ ﾑ ﾒ ﾓ ﾔ ﾕ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ ﾜ ﾝ ﾞ ﾟ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_true (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}

void
furiganafold_with_all_hiragana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖ゛゜ゝゞゟ");
    GError * error = NULL;
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 10);
    lw_utf8_furiganafold (text, -1, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 0, 13, 26, 39, 52, 65, 79, 92, 100, 100};
    g_assert_cmpstr ("ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖ゛゜ゝゞゟ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_true (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}



gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/normalize/japanese_and_english", Fixture, NULL, setup, normalize_japanese_and_english, teardown);
    g_test_add ("/normalize/casefold_japanese_and_english", Fixture, NULL, setup, normalize_casefold_japanese_and_english, teardown);
    g_test_add ("/normalize/furiganafold_japanese_and_english", Fixture, NULL, setup, normalize_furiganafold_japanese_and_english, teardown);
    g_test_add ("/normalize/allfold_japanese_and_english", Fixture, NULL, setup, normalize_allfold_japanese_and_english, teardown);
    g_test_add ("/normalize/no_text", Fixture, NULL, setup, normalize_no_text, teardown);

    g_test_add ("/furiganafold/no_text", Fixture, NULL, setup, furiganafold_no_text, teardown);
    g_test_add ("/furiganafold/with_mixed_text", Fixture, NULL, setup, furiganafold_with_mixed_text, teardown);
    g_test_add ("/furiganafold/with_all_katakana", Fixture, NULL, setup, furiganafold_with_all_katakana, teardown);
    g_test_add ("/furiganafold/with_all_halfwidth_katakana", Fixture, NULL, setup, furiganafold_with_all_halfwidth_katakana, teardown);
    g_test_add ("/furiganafold/with_all_hiragana", Fixture, NULL, setup, furiganafold_with_all_hiragana, teardown);

    return g_test_run();
}



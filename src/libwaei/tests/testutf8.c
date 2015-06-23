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
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, 0, LW_UTF8FLAG_NONE);

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
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, 0, LW_UTF8FLAG_CASEFOLD);

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
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, 0, LW_UTF8FLAG_FURIGANAFOLD);

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
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, 0, LW_UTF8FLAG_ALL);

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
    gchar * normalized = NULL;

    //Initializations
    normalized = lw_utf8_normalize (TEXT, 0, LW_UTF8FLAG_NONE);

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
    gint i = 0;

    //Initializations
    lw_utf8_furiganafold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = {};
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
    gint i = 0;

    //Initializations
    lw_utf8_furiganafold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 8, 21, 47, 73, 100};
    g_assert_cmpstr ("abcDEFGあいうえおかきくけこ日本語", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}


void
furiganafold_with_all_katakana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶヷヸヹヺ・ーヽヾヿ");
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 10);
    lw_utf8_furiganafold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 12, 25, 37, 50, 63, 75, 88, 100};
    g_assert_cmpstr ("ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖヷヸヹヺ・ーゝゞゟ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}

void
furiganafold_with_all_halfwidth_katakana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("･ ｦ ｧ ｨ ｩ ｪ ｫ ｬ ｭ ｮ ｯｰ ｱ ｲ ｳ ｴ ｵ ｶ ｷ ｸ ｹ ｺ ｻ ｼ ｽ ｾ ｿﾀ ﾁ ﾂ ﾃ ﾄ ﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾋ ﾌ ﾍ ﾎ ﾏﾐ ﾑ ﾒ ﾓ ﾔ ﾕ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ ﾜ ﾝ ﾞ ﾟ");
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 10);
    lw_utf8_furiganafold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 10, 21, 31, 42, 52, 62, 74, 84, 94, 100};
    g_assert_cmpstr ("･ ｦ ｧ ｨ ｩ ｪ ｫ ｬ ｭ ｮ ｯｰ ｱ ｲ ｳ ｴ ｵ ｶ ｷ ｸ ｹ ｺ ｻ ｼ ｽ ｾ ｿﾀ ﾁ ﾂ ﾃ ﾄ ﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾋ ﾌ ﾍ ﾎ ﾏﾐ ﾑ ﾒ ﾓ ﾔ ﾕ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ ﾜ ﾝ ﾞ ﾟ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}

void
furiganafold_with_all_hiragana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖ゛゜ゝゞゟ");
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 10);
    lw_utf8_furiganafold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 13, 26, 39, 52, 65, 79, 92, 100};
    g_assert_cmpstr ("ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖ゛゜ゝゞゟ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}


void
furiganafold_where_cancels_halfway (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ァアィイゥウェエォオ");
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 1);
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);
    lw_utf8_furiganafold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 30, 60};
    g_assert_cmpstr ("ぁあぃいぅうぇえぉオ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_assert_true (lw_progress_is_cancelled (fixture->progress));

    g_free (text);
    text = NULL;
}


void
furiganafold_where_progress_is_null (Fixture * fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ァアィイゥウェエォオ");
    gint i = 0;

    //Initializations
    lw_utf8_furiganafold (text, 0, NULL);

    //Assert
    gint expected_steps[] = { };
    g_assert_cmpstr ("ぁあぃいぅうぇえぉお", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_assert_false (lw_progress_is_cancelled (fixture->progress));

    g_free (text);
    text = NULL;
}


void
casefold_no_text (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("");

    //Initializations
    lw_utf8_casefold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = {};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    g_assert_cmpstr ("", ==, text);

    g_free (text);
    text = NULL;
}


void
casefold_with_mixed_text (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("abcDEFGあいうえおカキクケコ日本語");
    gint i = 0;

    //Initializations
    lw_utf8_casefold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 8, 21, 47, 73, 100};
    g_assert_cmpstr ("abcdefgあいうえおカキクケコ日本語", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}


void
casefold_with_uppercase_letters (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    gint i = 0;

    //Initializations
    lw_utf8_casefold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 15, 30, 46, 61, 76, 92, 100};
    g_assert_cmpstr ("abcdefghijklmnopqrstuvwxyz", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}


void
casefold_with_lowercase_letters (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("abcdefghijklmnopqrstuvwxyz");
    gint i = 0;

    //Initializations
    lw_utf8_casefold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = { 0, 15, 30, 46, 61, 76, 92, 100};
    g_assert_cmpstr ("abcdefghijklmnopqrstuvwxyz", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));

    g_free (text);
    text = NULL;
}


void
casefold_where_cancels_halfway (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 1);
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);
    lw_utf8_casefold (text, 0, fixture->progress);

    //Assert
    gint expected_steps[] = {0, 11, 23, 34, 46, 57};
    g_assert_cmpstr ("abcdefghijklmnopqrSTUVWXYZ", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_assert_true (lw_progress_is_cancelled (fixture->progress));

    g_free (text);
    text = NULL;
}


void
casefold_where_progress_is_null (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar * text = g_strdup("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    gint i = 0;

    //Initializations
    lw_progress_set_prefered_chunk_size (fixture->progress, 1);
    lw_utf8_casefold (text, 0, NULL);

    //Assert
    gint expected_steps[] = {};
    g_assert_cmpstr ("abcdefghijklmnopqrstuvwxyz", ==, text);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_assert_false (lw_progress_is_cancelled (fixture->progress));

    g_free (text);
    text = NULL;
}


void
contains_kanji_where_empty (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_kanji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_kanji_where_only_kanji (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_kanji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
contains_kanji_with_nothing_kanji (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcあいうえおアイウエオ";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_kanji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_kanji_with_mix (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcあいうえおアイウエオ日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_kanji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
contains_furigana_where_empty (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_furigana (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_furigana_where_only_hiragana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "あいうえお";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_furigana (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
contains_furigana_where_only_katakana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "アイウエオ";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_furigana (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
contains_furigana_with_nothing_furigana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abc日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_furigana (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_furigana_with_mix (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcあいうえおアイウエオ日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_furigana (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
contains_romaji_where_empty (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_romaji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_romaji_where_only_hiragana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "あいうえお";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_romaji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_romaji_where_only_katakana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "アイウエオ";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_romaji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_romaji_with_nothing_romaji (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "あいうえおアイウエオ日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_romaji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_romaji_with_mix (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcあいうえおアイウエオ日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_romaji (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
contains_number_where_empty (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_number (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_number_with_only_numbers (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "0123";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_number (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
contains_number_where_only_hiragana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "あいうえお";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_number (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_number_where_only_katakana (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "アイウエオ";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_number (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_number_with_no_numbers (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "あいうえおアイウエオ日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_number (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
contains_number_with_mix (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "0123abcあいうえおアイウエオ日本語";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_contains_number (TEXT);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
isescaped_where_empty (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_isescaped (TEXT, TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
isescaped_where_unescaped (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "a";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_isescaped (TEXT, TEXT);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
isescaped_where_escaped (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "\\a";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_isescaped (TEXT, TEXT + 1);

    //Assert
    g_assert_cmpint (contains, ==, TRUE);
}


void
isescaped_where_double_escaped (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "\\\\a";
    gboolean contains = FALSE;

    //Initializations
    contains = lw_utf8_isescaped (TEXT, TEXT + 2);

    //Assert
    g_assert_cmpint (contains, ==, FALSE);
}


void
validate_valid_string (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcdあいうえおアイウエオ日本語";
    gint i = 0;
    gboolean is_valid = FALSE;
    LwProgress * progress = fixture->progress;

    //Initializations
    is_valid = lw_utf8_validate (TEXT, 0, progress);

    //Assert
    gint expected_steps[] = { 0, 4, 16, 37, 58, 79, 100};
    g_assert_true (is_valid);
    g_assert_no_error (lw_progress_get_error (progress));
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
}


void
validate_last_character_is_invalid (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "彙abcdあいうえおアイウエオ日本語\xF0";
    gint i = 0;
    gboolean is_valid = FALSE;
    LwProgress * progress = fixture->progress;

    //Initializations
    is_valid = lw_utf8_validate (TEXT, 0, progress);

    //Assert
    gint expected_steps[] = { 0, 8, 14, 34, 53, 72, 91};
    g_assert_false (is_valid);
    g_assert_error (lw_progress_get_error (progress), LW_UTF8_ERROR, LW_UTF8_ERRORCODE_VALIDATION_ERROR);
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
}

void
validate_last_invalid_character_is_not_validated (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcd\xF0";
    gint i = 0;
    gboolean is_valid = FALSE;
    LwProgress * progress = fixture->progress;

    //Initializations
    is_valid = lw_utf8_validate (TEXT, 4, progress);

    //Assert
    gint expected_steps[] = { 0, 50, 100};
    g_assert_true (is_valid);
    g_assert_no_error (lw_progress_get_error (progress));
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
}


void
validate_where_progress_is_null (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcd";
    gboolean is_valid = FALSE;

    //Initializations
    is_valid = lw_utf8_validate (TEXT, 0, NULL);

    //Assert
    g_assert_true (is_valid);
}

void
validate_where_progress_is_null_with_invalid_string (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "abcd\xF0";
    gboolean is_valid = FALSE;

    //Initializations
    is_valid = lw_utf8_validate (TEXT, 0, NULL);

    //Assert
    g_assert_false (is_valid);
}


void
validate_where_progress_is_cancelled (Fixture *fixture, gconstpointer data)
{
    //Declarations
    gchar const * TEXT = "彙abcdあいうえおアイウエオ日本語\xF0";
    gboolean is_valid = FALSE;
    gint i = 0;
    LwProgress * progress = fixture->progress;
    lw_progress_set_prefered_chunk_size (fixture->progress, 1);
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);

    //Initializations
    is_valid = lw_utf8_validate (TEXT, 0, progress);

    //Assert
    gint expected_steps[] = { 0, 6, 10, 14, 27, 40, 53};
    g_assert_false (is_valid);
    g_assert_no_error (lw_progress_get_error (progress));
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
}


void
replace_linebreaks_with_nullcharacter_when_blank_string (Fixture *fixture, gconstpointer data)
{
    //Arrange
    gchar * text = g_strdup ("");
    gsize max_line_length = 0;
    gint i = 0;
    LwProgress * progress = fixture->progress;

    //Act
    lw_utf8_replace_linebreaks_with_nullcharacter (text, strlen(text), &max_line_length, progress);

    //Assert
    g_assert_true (memcmp("", text, strlen("")) == 0);
    gint expected_steps[] = {};
    g_assert_no_error (lw_progress_get_error (progress));
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_assert_false (lw_progress_is_cancelled (progress));

    g_free (text);
}


void
replace_linebreaks_with_nullcharacter_when_multiple_lines (Fixture *fixture, gconstpointer data)
{
    //Arrange
    gchar * text = g_strdup ("one\ntwo\nthree");
    gsize max_line_length = 0;
    gint i = 0;
    LwProgress * progress = fixture->progress;

    //Act
    lw_utf8_replace_linebreaks_with_nullcharacter (text, strlen(text), &max_line_length, progress);

    //Assert
    g_assert_true (memcmp("one\0two\0three", text, strlen("one\ntwo\nthree")) == 0);
    gint expected_steps[] = {0, 15, 38, 53, 69, 84, 100};
    g_assert_no_error (lw_progress_get_error (progress));
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_assert_false (lw_progress_is_cancelled (progress));

    g_free (text);
}


void
replace_linebreaks_with_nullcharacter_when_only_one_line (Fixture *fixture, gconstpointer data)
{
    //Arrange
    gchar * text = g_strdup ("one two three");
    gsize max_line_length = 0;
    gint i = 0;
    LwProgress * progress = fixture->progress;

    //Act
    lw_utf8_replace_linebreaks_with_nullcharacter (text, strlen(text), &max_line_length, progress);

    //Assert
    g_assert_true (memcmp("one two three", text, strlen("one two three")) == 0);
    gint expected_steps[] = {0, 15, 30, 46, 61, 76, 92, 100};
    g_assert_no_error (lw_progress_get_error (progress));
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_false (lw_progress_completed (fixture->progress));
    g_assert_false (lw_progress_is_cancelled (progress));

    g_free (text);
}


void
replace_linebreaks_with_nullcharacter_when_no_progress (Fixture *fixture, gconstpointer data)
{
    //Arrange
    gchar * text = g_strdup ("one\ntwo\nthree");
    gsize max_line_length = 0;
    gint i = 0;
    LwProgress * progress = NULL;

    //Act
    lw_utf8_replace_linebreaks_with_nullcharacter (text, strlen(text), &max_line_length, progress);

    //Assert
    g_assert_true (memcmp("one\0two\0three", text, strlen("one\ntwo\nthree")) == 0);
    gint expected_steps[] = {};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }

    g_free (text);
}


void
replace_linebreaks_with_nullcharacter_when_cancelled (Fixture *fixture, gconstpointer data)
{
    //Arrange
    gchar * text = g_strdup ("one\ntwo\nthree");
    gsize max_line_length = 0;
    gint i = 0;
    LwProgress * progress = fixture->progress;
    g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (cancel_progress), fixture);

    //Act
    lw_utf8_replace_linebreaks_with_nullcharacter (text, strlen(text), &max_line_length, progress);

    //Assert
    g_assert_true (memcmp("one\0two\0three", text, strlen("one\ntwo\nthree")) == 0);
    gint expected_steps[] = {0, 15, 38, 53};
    g_assert_cmpint (fixture->steps->len, ==, G_N_ELEMENTS (expected_steps));
    for (i = 0; i < fixture->steps->len; i++)
    {
      g_assert_cmpint (g_array_index (fixture->steps, gint, i), ==, expected_steps[i]);
    }
    g_assert_true (lw_progress_is_cancelled (progress));

    g_free (text);
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
    g_test_add ("/furiganafold/where_cancels_halfway", Fixture, NULL, setup, furiganafold_where_cancels_halfway, teardown);
    g_test_add ("/furiganafold/where_progress_is_null", Fixture, NULL, setup, furiganafold_where_progress_is_null, teardown);

    g_test_add ("/casefold/no_text", Fixture, NULL, setup, casefold_no_text, teardown);
    g_test_add ("/casefold/with_mixed_text", Fixture, NULL, setup, casefold_with_mixed_text, teardown);
    g_test_add ("/casefold/with_uppercase_letters", Fixture, NULL, setup, casefold_with_uppercase_letters, teardown);
    g_test_add ("/casefold/with_lowercase_letters", Fixture, NULL, setup, casefold_with_lowercase_letters, teardown);
    g_test_add ("/casefold/where_cancels_halfway", Fixture, NULL, setup, casefold_where_cancels_halfway, teardown);
    g_test_add ("/casefold/where_progress_is_null", Fixture, NULL, setup, casefold_where_progress_is_null, teardown);

    g_test_add ("/contains_kanji/where_empty", Fixture, NULL, setup, contains_kanji_where_empty, teardown);
    g_test_add ("/contains_kanji/where_only_kanji", Fixture, NULL, setup, contains_kanji_where_only_kanji, teardown);
    g_test_add ("/contains_kanji/with_nothing_kanji", Fixture, NULL, setup, contains_kanji_with_nothing_kanji, teardown);
    g_test_add ("/contains_kanji/with_mix", Fixture, NULL, setup, contains_kanji_with_mix, teardown);

    g_test_add ("/contains_furigana/where_empty", Fixture, NULL, setup, contains_furigana_where_empty, teardown);
    g_test_add ("/contains_furigana/where_only_hiragana", Fixture, NULL, setup, contains_furigana_where_only_hiragana, teardown);
    g_test_add ("/contains_furigana/where_only_katakana", Fixture, NULL, setup, contains_furigana_where_only_katakana, teardown);
    g_test_add ("/contains_furigana/with_nothing_furigana", Fixture, NULL, setup, contains_furigana_with_nothing_furigana, teardown);
    g_test_add ("/contains_furigana/with_mix", Fixture, NULL, setup, contains_furigana_with_mix, teardown);

    g_test_add ("/contains_romaji/where_empty", Fixture, NULL, setup, contains_romaji_where_empty, teardown);
    g_test_add ("/contains_romaji/where_only_hiragana", Fixture, NULL, setup, contains_romaji_where_only_hiragana, teardown);
    g_test_add ("/contains_romaji/where_only_katakana", Fixture, NULL, setup, contains_romaji_where_only_katakana, teardown);
    g_test_add ("/contains_romaji/with_nothing_romaji", Fixture, NULL, setup, contains_romaji_with_nothing_romaji, teardown);
    g_test_add ("/contains_romaji/with_mix", Fixture, NULL, setup, contains_romaji_with_mix, teardown);

    g_test_add ("/contains_number/where_empty", Fixture, NULL, setup, contains_number_where_empty, teardown);
    g_test_add ("/contains_number/with_only_numbers", Fixture, NULL, setup, contains_number_with_only_numbers, teardown);
    g_test_add ("/contains_number/where_only_hiragana", Fixture, NULL, setup, contains_number_where_only_hiragana, teardown);
    g_test_add ("/contains_number/where_only_katakana", Fixture, NULL, setup, contains_number_where_only_katakana, teardown);
    g_test_add ("/contains_number/with_no_numbers", Fixture, NULL, setup, contains_number_with_no_numbers, teardown);
    g_test_add ("/contains_number/with_mix", Fixture, NULL, setup, contains_number_with_mix, teardown);

    g_test_add ("/isescaped/where_empty", Fixture, NULL, setup, isescaped_where_empty, teardown);
    g_test_add ("/isescaped/where_unexcaped", Fixture, NULL, setup, isescaped_where_unescaped, teardown);
    g_test_add ("/isescaped/where_escaped", Fixture, NULL, setup, isescaped_where_escaped, teardown);
    g_test_add ("/isescaped/where_double_escaped", Fixture, NULL, setup, isescaped_where_double_escaped, teardown);

    g_test_add ("/validate/valid_string", Fixture, NULL, setup, validate_valid_string, teardown);
    g_test_add ("/validate/last_character_is_invalid", Fixture, NULL, setup, validate_last_character_is_invalid, teardown);
    g_test_add ("/validate/last_invalid_character_is_not_validated", Fixture, NULL, setup, validate_last_invalid_character_is_not_validated, teardown);
    g_test_add ("/validate/where_progress_is_null", Fixture, NULL, setup, validate_where_progress_is_null, teardown);
    g_test_add ("/validate/where_progress_is_null_with_invalid_string", Fixture, NULL, setup, validate_where_progress_is_null_with_invalid_string, teardown);
    g_test_add ("/validate/where_progress_is_cancelled", Fixture, NULL, setup, validate_where_progress_is_cancelled, teardown);

    g_test_add ("/replace_linebreaks_with_nullcharacter/when_blank_string", Fixture, NULL, setup, replace_linebreaks_with_nullcharacter_when_blank_string, teardown);
    g_test_add ("/replace_linebreaks_with_nullcharacter/when_multiple_lines", Fixture, NULL, setup, replace_linebreaks_with_nullcharacter_when_multiple_lines, teardown);
    g_test_add ("/replace_linebreaks_with_nullcharacter/when_only_one_line", Fixture, NULL, setup, replace_linebreaks_with_nullcharacter_when_only_one_line, teardown);
    g_test_add ("/replace_linebreaks_with_nullcharacter/when_no_progress", Fixture, NULL, setup, replace_linebreaks_with_nullcharacter_when_no_progress, teardown);
    g_test_add ("/replace_linebreaks_with_nullcharacter/when_cancelled", Fixture, NULL, setup, replace_linebreaks_with_nullcharacter_when_cancelled, teardown);
/*
lw_utf8_replace_linebreaks_with_nullcharacter
lw_utf8_sanitize
lw_utf8_normalize_chunked
lw_utf8flag_clean
lw_utf8_set_null_next_char
*/


    return g_test_run ();
}

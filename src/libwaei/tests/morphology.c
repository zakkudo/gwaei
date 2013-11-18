#include <string.h>
#include <glib.h>
#include <libwaei/libwaei.h>


struct _MorphologyFixture {
  LwMorphologyEngine *engine;
};
typedef struct _MorphologyFixture MorphologyFixture;


void
morphology_test_setup (MorphologyFixture *fixture, 
                       gconstpointer      data)
{
    memset(fixture, 0, sizeof(MorphologyFixture));
    fixture->engine = lw_morphologyengine_new ("en_US");
}


void
morphology_test_teardown (MorphologyFixture *fixture, 
                          gconstpointer      data)
{
    g_object_unref (fixture->engine); fixture->engine = NULL;
}


void
morphology_test_english (MorphologyFixture *fixture, 
                         gconstpointer      data)
{
    const gchar* TEXT = "I was going to the store quickly.";
    LwMorphologyList *list = lw_morphologyengine_analyze (fixture->engine, TEXT, FALSE);

/*
    {
      gchar *test = lw_morphologylist_to_string (list);
      printf("\n\nBREAK test English \n%s\n\n", test);
    }
*/

    LwMorphology *morphology = NULL;

/*
    while ((morphology = lw_morphologylist_read (list)) != NULL)
    {
      if (morphology->word != NULL) printf("BREAK %s\n", morphology->word);
      if (morphology->base_form != NULL) printf("BREAK %s\n", morphology->base_form);
    }
*/

    morphology = lw_morphologylist_find (list, "quickly");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "quick");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "going");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "go");
    g_assert (morphology != NULL);
}


void
morphology_test_japanese (MorphologyFixture *fixture, 
                          gconstpointer      data)
{
    const gchar* TEXT = "私がお店で出かけていました。";
    LwMorphologyList *list = lw_morphologyengine_analyze (fixture->engine, TEXT, FALSE);
    g_assert (list != NULL);
    LwMorphology *morphology = NULL;

/*
    {
      gchar *test = lw_morphologylist_to_string (list);
      printf("\n\nBREAK test Japanese \n%s\n\n", test);
    }
*/

    morphology = lw_morphologylist_find (list, "出かける");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "店");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "私");
    g_assert (morphology != NULL);

}


void
morphology_test_mix (MorphologyFixture *fixture, 
                     gconstpointer      data)
{
    const gchar* TEXT = "日本語 is a Japanese word.";
    LwMorphologyList *list = lw_morphologyengine_analyze (fixture->engine, TEXT, FALSE);
    LwMorphology *morphology = NULL;

/*
    {
      gchar *test = lw_morphologylist_to_string (list);
      printf("\n\nBREAK test english \n%s\n\n", test);
    }
*/

    morphology = lw_morphologylist_find (list, "日本語");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "is");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "Japanese");
    g_assert (morphology != NULL);

    morphology = lw_morphologylist_find (list, "word");
    g_assert (morphology != NULL);
}


void
morphology_test_edictionary_lines (MorphologyFixture *fixture, 
                                   gconstpointer      data)
{
    const gchar* TEXT = 
      "２進化１０進法 [にしんかじっしんほう] /(n) (comp) binary-coded decimal notation/\n"
      "２進加算器 [にしんかさんき] /(n) (comp) binary adder/\n"
      "２進数 [にしんすう] /(n) binary digits/\n"
      "ＵＲＬ [ユーアールエル] /(n) uniform resouce locator/URL/\n"
      "アーカイブビット /(n) (comp) archive bit/\n"
      "うざっこい /(adj-i) jumbled up/confused/frustrating/";
    LwMorphologyList *list = lw_morphologyengine_analyze (fixture->engine, TEXT, FALSE);
    LwMorphology *morphology = NULL;
    const gchar* TERM_SHOULD_EXIST[] = {
      "binary-coded",
      "notation",
      //"にしんかさんき",
      "ユーアールエル",
      "アーカイブ",
      "ビット",
      //"うざっこい",
      "ＵＲＬ",
      "URL",
      //"n",
      //"adj-i",
      NULL
    };
    gint i;

  /*
    {
      gchar *test = lw_morphologylist_to_string (list);
      printf("\n\nBREAK test edictionary lines \n%s\n\n", test);
    }
  */

    for (i = 0; TERM_SHOULD_EXIST[i] != NULL; i++)
    {
      morphology = lw_morphologylist_find (list, TERM_SHOULD_EXIST[i]);
      if (morphology == NULL) g_error ("Failed on %s", TERM_SHOULD_EXIST[i]);
      //g_assert (morphology != NULL);
    }
}

void
morphology_test_kanjidictionary_lines (MorphologyFixture *fixture, 
                                       gconstpointer      data)
{
    const gchar* TEXT =
      "亜 ｜ 一 口 3021 U4e9c B1 C7 G8 S7 XJ05033 F1509 J1 N43 V81 H3540 DK2204 L1809 K1331 O525 DO1788 MN272 MP1.0525 E997 IN1616 DF1032 DT1092 DJ1818 DG35 DM1827 P4-7-1 I0a7.14 Q1010.6 DR3273 Yya4 Wa ア つ.ぐ T1 や つぎ つぐ {Asia} {rank next} {come after} {-ous}\n"
      "唖 ｜ 一 口 3022 U5516 B30 S10 XJ13560 XJ14D64 N939 V795 L2958 MN3743 MP2.1066 P1-3-7 I3d8.3 Q6101.7 Yya1 Wa ア アク おし {mute} {dumb}\n"
      "娃 女 土 3023 U5a03 B38 G9 S9 V1213 L2200 MN6262 MP3.0707 P1-3-6 I3e6.5 Q4441.4 Ywa2 Wwae Wwa ア アイ ワ うつく.しい T1 い {beautiful}\n"
      "阿 一 口 亅 阡 3024 U963f B170 G9 S8 XN5008 F1126 J1 N4985 V6435 H346 DK256 L1295 K1515 O569 MN41599 MP11.0798 IN2258 DM1304 P1-3-5 I2d5.6 Q7122.0 Ya1 Ye1 Ya5 Ya2 Ya4 Wa Wog ア オ おもね.る くま T1 ほとり あず あわ おか きた な {Africa} {flatter} {fawn upon} {corner} {nook} {recess}\n"
      "哀 衣 口 亠 3025 U54c0 B8 C30 G8 S9 F1715 J1 N304 V791 H2068 DK1310 L401 K1670 DO1249 MN3580 MP2.0997 E998 IN1675 DF1131 DT1239 DJ1804 DG327 DM408 P2-2-7 I2j7.4 Q0073.2 DR465 Yai1 Wae アイ あわ.れ あわ.れむ かな.しい {pathetic} {grief} {sorrow} {pathos} {pity} {sympathize}\n"
      "愛 心 爪 冖 夂 3026 U611b B87 C61 G4 S13 F640 J2 N2829 V1927 H2492 DK1606 L737 K436 O2018 DO456 MN10947 MP4.1123 E417 IN259 DS339 DF545 DH441 DT602 DC268 DJ1079 DG790 DM745 P2-4-9 I4i10.1 Q2024.7 DR2067 ZPP2-1-12 ZPP2-6-7 Yai4 Wae アイ いと.しい かな.しい め.でる お.しむ まな T1 あ あし え かな なる めぐ めぐみ よ>し ちか {love} {affection} {favourite}\n";
      LwMorphologyList *list = lw_morphologyengine_analyze (fixture->engine, TEXT, FALSE);
      LwMorphology *morphology = NULL;

    const gchar* TERM_SHOULD_EXIST[] = {
      "3021",
      "C30",
      "Yai4",
      "亜",
      //"ZPP2-1-12",
      "なる",
      "affection",
      "いとしい",
      NULL
    };
    gint i;

  /*
    {
      gchar *test = lw_morphologylist_to_string (list);
      printf("\n\nBREAK test kanjidictionary lines \n%s\n\n", test);
    }
  */

    for (i = 0; TERM_SHOULD_EXIST[i] != NULL; i++)
    {
      morphology = lw_morphologylist_find (list, TERM_SHOULD_EXIST[i]);
      g_assert (morphology != NULL);
    }

}

void
morphology_test_exampledictionary_lines (MorphologyFixture *fixture, 
                                         gconstpointer      data)
{
    const gchar* TEXT =
      "A: 「これが探していたものだ」と彼は叫んだ。 \"This is what I was looking for!\" he exclaimed.#ID=1513_4924\n"
      "B: 此れ[01]{これ} が 探す{探していた} 物(もの)[01]{もの} だ と 彼(かれ)[01] は[01] 叫ぶ{叫んだ}\n"
      "A: 「これはとてもおもしろそうだね」とひろしが言います。 \"This looks pretty interesting,\" Hiroshi says.#ID=1514_4925\n"
      "B: 此れ[01]{これ} は[01] 迚も[01]{とても}~ 面白い{おもしろ} そう[02] だ ね[01] と[04] が 言う{言います}\n"
      "A: 彼らのコミュニケーションは我々が考えてきたものよりはるかに複雑かもしれません。 Their communication may be much more complex than we thought.#ID=1515_4926\n"
      "B: 彼ら(かれら){彼らの} コミュニケーション は 我々 が 考える{考えて} 来る(くる){きた} 物(もの){もの} より 遥か{はるか} に 複雑 かも知れない{かもしれません}\n"
      "A: またいつか風のように走るんだ。 Someday I'll run like the wind.#ID=1519_4930\n"
      "B: 又{また} 何時か{いつか} 風(かぜ) の様に[01]{のように} 走る[01] のだ{んだ}\n"
      "A: お願いだから泣かないで。 Please don't cry.#ID=1521_4932\n"
      "B: お願い[02] だから 泣く{泣かないで}\n";
    LwMorphologyList *list = lw_morphologyengine_analyze (fixture->engine, TEXT, FALSE);
    LwMorphology *morphology = NULL;

    const gchar* TERM_SHOULD_EXIST[] = {
      "彼ら",
      "コミュニケーション",
      "我々",
      "Someday",
      "Please",
      "wind",
      NULL
    };
    gint i;

  /*
    {
      gchar *test = lw_morphologylist_to_string (list);
      printf("\n\nBREAK test exampledictionary lines \n%s\n\n", test);
    }
  */

    for (i = 0; TERM_SHOULD_EXIST[i] != NULL; i++)
    {
      morphology = lw_morphologylist_find (list, TERM_SHOULD_EXIST[i]);
      g_assert (morphology != NULL);
    }
}


gint
main (gint argc, gchar *argv[])
{
/*TODO
    g_type_init ();
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/libwaei/morphology/english", MorphologyFixture, NULL, morphology_test_setup, morphology_test_english, morphology_test_teardown);
    g_test_add ("/libwaei/morphology/japanese", MorphologyFixture, NULL, morphology_test_setup, morphology_test_japanese, morphology_test_teardown);
    g_test_add ("/libwaei/morphology/mix", MorphologyFixture, NULL, morphology_test_setup, morphology_test_mix, morphology_test_teardown);
    g_test_add ("/libwaei/morphology/edictionary_lines", MorphologyFixture, NULL, morphology_test_setup, morphology_test_edictionary_lines, morphology_test_teardown);
    g_test_add ("/libwaei/morphology/kanjidictionary_lines", MorphologyFixture, NULL, morphology_test_setup, morphology_test_kanjidictionary_lines, morphology_test_teardown);
    g_test_add ("/libwaei/morphology/exampledictionary_lines", MorphologyFixture, NULL, morphology_test_setup, morphology_test_exampledictionary_lines, morphology_test_teardown);

  return g_test_run();
*/ return 0;
}



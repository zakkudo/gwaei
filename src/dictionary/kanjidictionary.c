/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Zachary Dovel. All Rights Reserved.

    LICENSE:
    This file is part of gWaei.

    gWaei is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gWaei is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with gWaei.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

//!
//!  @file kanjidictionary.c
//!
//!  @brief LwKanjiDictionary objects represent a loaded dictionary that the program
//!         can use to carry out searches.  You can uninstall dictionaries
//!         by using the object, but you cannot install them. LwDictInst
//!         objects exist for that purpose.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib-object.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>
#include <libwaei/dictionary-private.h>

#include <libwaei/dictionary/kanjidictionary.h>

G_DEFINE_DYNAMIC_TYPE (LwKanjiDictionary, lw_kanjidictionary, LW_TYPE_DICTIONARY)

static gint lw_kanjidictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_kanjidictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_kanjidictionary_get_column_handling (LwDictionary *self, gint column_num);
static gchar** lw_kanjidictionary_columnize (LwDictionary *self, gchar *buffer, gchar **columns, gsize *num_columns);
static void lw_kanjidictionary_load_columns (LwDictionary *self, gchar *buffer, gchar **tokens, gint num_tokens, LwParsedLine *line);
static gint * lw_kanjidictionary_calculate_applicable_columns_for_text (LwDictionary * self, gchar const * TEXT);


LwDictionary* lw_kanjidictionary_new (const gchar        *FILENAME, 
                                      LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_KANJIDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                NULL));

    return dictionary;
}


static void 
lw_kanjidictionary_init (LwKanjiDictionary *kanjidictionary)
{
}


static void
lw_kanjidictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_kanjidictionary_parent_class)->constructed (object);
    }

    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (strcmp(priv->filename, "Kanji") == 0)
    {
      if (priv->name != NULL) g_free (priv->name); priv->name = NULL;
      priv->name = g_strdup (gettext("Kanji"));
    }

}


static void 
lw_kanjidictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_kanjidictionary_parent_class)->finalize (object);
}


static void
lw_kanjidictionary_class_finalize (LwKanjiDictionaryClass *klass)
{
}


static void
lw_kanjidictionary_class_init (LwKanjiDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_kanjidictionary_finalize;
    object_class->constructed = lw_kanjidictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->priv->get_column_handling = lw_kanjidictionary_get_column_handling;
    dictionary_class->priv->get_total_columns = lw_kanjidictionary_get_total_columns;
    dictionary_class->priv->get_column_language = lw_kanjidictionary_get_column_language;
    dictionary_class->priv->columnize = lw_kanjidictionary_columnize;
    dictionary_class->priv->load_columns = lw_kanjidictionary_load_columns;
    dictionary_class->priv->calculate_applicable_columns_for_text = lw_kanjidictionary_calculate_applicable_columns_for_text;
    dictionary_class->priv->columnid_get_type = lw_kanjidictionary_columnid_get_type;
}


static gint
lw_kanjidictionary_get_total_columns (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_KANJIDICTIONARY (self), 0);

    return TOTAL_LW_KANJIDICTIONARYCOLUMNIDS;
}



static gchar const *
lw_kanjidictionary_get_column_language (LwDictionary *self,
                                    gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_KANJIDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_KANJIDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gchar const * column_languages[TOTAL_LW_KANJIDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_languages[LW_KANJIDICTIONARYCOLUMNID_KANJI] = "ja";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL] = "symbol";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY] = "number";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT] = "number";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL] = "number";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL] = "number";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS] = "ja";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_ON_READINGS] = "ja";
      column_languages[LW_KANJIDICTIONARYCOLUMNID_MEANINGS] = "en";
    }

    return column_languages[column_num];
}


static LwDictionaryColumnHandling
lw_kanjidictionary_get_column_handling (LwDictionary *self,
                                    gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_KANJIDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_KANJIDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gint column_handlings[TOTAL_LW_KANJIDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_KANJI] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_ON_READINGS] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_KANJIDICTIONARYCOLUMNID_MEANINGS] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
    }

    return column_handlings[column_num];
}


GType
lw_kanjidictionary_columnid_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_KANJIDICTIONARYCOLUMNID_KANJI, LW_KANJIDICTIONARYCOLUMNNAME_KANJI, LW_KANJIDICTIONARYCOLUMNNICK_KANJI },
        { LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL, LW_KANJIDICTIONARYCOLUMNNAME_UNICODE_SYMBOL, LW_KANJIDICTIONARYCOLUMNNICK_UNICODE_SYMBOL },
        { LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY, LW_KANJIDICTIONARYCOLUMNNAME_USAGE_FREQUENCY, LW_KANJIDICTIONARYCOLUMNNICK_USAGE_FREQUENCY },
        { LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT, LW_KANJIDICTIONARYCOLUMNNAME_STROKE_COUNT, LW_KANJIDICTIONARYCOLUMNNICK_STROKE_COUNT },
        { LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL, LW_KANJIDICTIONARYCOLUMNNAME_GRADE_LEVEL, LW_KANJIDICTIONARYCOLUMNNICK_GRADE_LEVEL },
        { LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL, LW_KANJIDICTIONARYCOLUMNNAME_JLPT_LEVEL, LW_KANJIDICTIONARYCOLUMNNICK_JLPT_LEVEL },
        { LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS, LW_KANJIDICTIONARYCOLUMNNAME_KUN_READINGS, LW_KANJIDICTIONARYCOLUMNNICK_KUN_READINGS },
        { LW_KANJIDICTIONARYCOLUMNID_ON_READINGS, LW_KANJIDICTIONARYCOLUMNNAME_ON_READINGS, LW_KANJIDICTIONARYCOLUMNNICK_ON_READINGS },
        { LW_KANJIDICTIONARYCOLUMNID_MEANINGS, LW_KANJIDICTIONARYCOLUMNNAME_MEANINGS, LW_KANJIDICTIONARYCOLUMNNICK_MEANINGS },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwKanjiDictionaryColumnId", values);
    }

    return type;
}


static gchar**
lw_kanjidictionary_columnize (LwDictionary  *self,
                              gchar         *buffer,
                              gchar        **columns,
                              gsize         *num_columns)
{
    //Sanity checks
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (columns != NULL, NULL);

    //逢 3029 U9022 B162 G9 S10 F2116 N4694 V6054 DP4002 DL2774 L2417 DN2497 O1516 MN38901X MP11.0075 P3-3-7 I2q7.15 Q3730.4 DR2555 ZRP3-4-7 Yfeng2 Wbong ホウ あ.う T1 おう {meeting}  

    //Declarations
    gchar *c = NULL;
    gchar *e = NULL;
    gint length = 0;

    //Initializations
    c = buffer;
    if (c == NULL) goto errored;

    while (*c != '\0')
    {
      while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c); //Find the next column start
      if (*c == '\0') break;

      if (*c == '{') //Columns can be multiple words long between {}
      {
        while (*c != '\0' && *c == '{') c = g_utf8_next_char (c);
        if (*c == '\0') break;

        e = c;
        {
          while (*e != '\0' && *e != '}') e = g_utf8_next_char (e); //Make sure the end of the column exists }
          if (*e == '\0') break;
    
          if (e - c > 1) columns[length++] = c;
        }
        c = lw_utf8_set_null_next_char (e);
        if (*c == '\0') break;
      }
      else //Every space denotes a column
      {
        e = c;
        {
          while (*e != '\0' && !g_ascii_isspace (*e)) e = g_utf8_next_char (e);

          if (e - c > 1) columns[length++] = c;
        }
        c = lw_utf8_set_null_next_char (e);
        if (*c == '\0') break;
      }
    }

    if (num_columns != NULL)
    {
      *num_columns = length;
    }

errored:

    columns[length] = NULL;

    return columns + length;
}


static gboolean
_is_unicode_symbol (gchar const *C)
{
    return (C[0] == 'U' &&
      g_ascii_isalnum (C[1]) &&
      g_ascii_isalnum (C[2]) &&
      g_ascii_isalnum (C[3]) &&
      g_ascii_isalnum (C[4]) &&
      (C[5] == '\0' || g_ascii_isspace (C[5])));
}

static gboolean
_is_stroke_count (gchar const *C)
{
    if (*C != 'S') return FALSE;
    C++;
    if (!g_ascii_isdigit (*C)) return FALSE;
    C++;
    while (g_ascii_isdigit (*C)) C++;
    return (*C == '\0' || g_ascii_isspace (*C));
}


static gboolean
_is_grade_level (gchar const *C)
{
    if (*C != 'G') return FALSE;
    C++;
    if (!g_ascii_isdigit (*C)) return FALSE;
    C++;
    while (g_ascii_isdigit (*C)) C++;
    return (*C == '\0' || g_ascii_isspace (*C));
}


static gboolean
_is_jlpt_level (gchar const *C)
{
    if (*C != 'J') return FALSE;
    C++;
    if (!g_ascii_isdigit (*C)) return FALSE;
    C++;
    while (g_ascii_isdigit (*C)) C++;
    return (*C == '\0' || g_ascii_isspace (*C));
}


static gboolean
_is_usage_frequency (gchar const *C)
{
    if (*C != 'F') return FALSE;
    C++;
    if (!g_ascii_isdigit (*C)) return FALSE;
    C++;
    while (g_ascii_isdigit (*C)) C++;
    return (*C == '\0' || g_ascii_isspace (*C));
}

static gboolean
_is_kun_reading (gchar const *C)
{
    if (*C == '\0') return FALSE;

    gboolean is_kun_reading = TRUE;

    while (*C != '\0' && is_kun_reading == TRUE)
    {
      gunichar c = g_utf8_get_char (C);
      GUnicodeScript script = g_unichar_get_script (c);
      if (script != G_UNICODE_SCRIPT_KATAKANA && !g_unichar_ispunct (c))
      {
        is_kun_reading = FALSE;
      }
      C = g_utf8_next_char (C);
    }

    return is_kun_reading;
}

static gboolean
_is_on_reading (gchar const *C)
{
    if (*C == '\0') return FALSE;

    gboolean is_on_reading = TRUE;

    while (*C != '\0' && is_on_reading == TRUE)
    {
      gunichar c = g_utf8_get_char (C);
      GUnicodeScript script = g_unichar_get_script (c);
      if (script != G_UNICODE_SCRIPT_HIRAGANA && !g_unichar_ispunct (c))
      {
        is_on_reading = FALSE;
      }
      C = g_utf8_next_char (C);
    }

    return is_on_reading;
}



static void
lw_kanjidictionary_load_columns (LwDictionary  *self,
                                 gchar         *buffer,
                                 gchar        **tokens,
                                 gint           num_tokens,
                                 LwParsedLine  *line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_KANJIDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (tokens != NULL);
    g_return_if_fail (num_tokens > 0);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *kanji = NULL;
    GArray *unicode_symbol = NULL;
    GArray *usage_frequency = NULL;
    GArray *stroke_count = NULL;
    GArray *grade_level = NULL;
    GArray *jlpt_level = NULL;
    GArray *kun_readings = NULL;
    GArray *on_readings = NULL;
    GArray *meanings = NULL;

    //Initializations
    kanji = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    unicode_symbol = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    usage_frequency = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    stroke_count = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    grade_level = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    jlpt_level = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    kun_readings = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    on_readings = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    meanings = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);

    { //Calculate the meanings now to make things easier on the forward iteration...
      gint i = num_tokens - 1;

      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう {meeting}  
      //                                                           ^
      //                                                           HERE

      do
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script == G_UNICODE_SCRIPT_KATAKANA || script == G_UNICODE_SCRIPT_HIRAGANA) break;

        if (tokens[i] != NULL) g_array_append_val (meanings, tokens[i]);
        i--;
      } while (i > 0);
      
      num_tokens = i + 1;
    }

    {
      gint i = 0;


      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう [THIS SECTION IS NOW INVISIBLE]
      //^
      //HERE

      if (tokens[i] != NULL && i < num_tokens) 
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script == G_UNICODE_SCRIPT_HAN)
        {
          g_array_append_val (kanji, tokens[i]);
          i++;
        }
      }

      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう [THIS SECTION IS NOW INVISIBLE]
      //   ^                                    ^
      //   HERE                                 END

      while (tokens[i] != NULL && i < num_tokens) 
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script == G_UNICODE_SCRIPT_KATAKANA || script == G_UNICODE_SCRIPT_HIRAGANA) break;

        if (_is_unicode_symbol (tokens[i]))
        {
          g_array_append_val (unicode_symbol, tokens[i]);
        }
        else if (_is_usage_frequency (tokens[i]))
        {
          g_array_append_val (usage_frequency, tokens[i]);
        }
        else if (_is_stroke_count (tokens[i]))
        {
          g_array_append_val (stroke_count, tokens[i]);
        }
        else if (_is_grade_level (tokens[i]))
        {
          g_array_append_val (grade_level, tokens[i]);
        }
        else if (_is_jlpt_level (tokens[i]))
        {
          g_array_append_val (jlpt_level, tokens[i]);
        }

        i++;
      }

      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう [THIS SECTION IS NOW INVISIBLE]
      //                                        ^          ^
      //                                        HERE       END

      while (tokens[i] != NULL && i < num_tokens) 
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script != G_UNICODE_SCRIPT_KATAKANA && script != G_UNICODE_SCRIPT_HIRAGANA) break;

        if (_is_kun_reading (tokens[i]))
        {
          g_array_append_val (kun_readings, tokens[i]);
        }
        else if (_is_on_reading (tokens[i]))
        {
          g_array_append_val (on_readings, tokens[i]);
        }


        i++;
      }

    }

errored:

    g_array_set_size (kanji, kanji->len);
    g_array_set_size (unicode_symbol, unicode_symbol->len);
    g_array_set_size (usage_frequency, usage_frequency->len);
    g_array_set_size (stroke_count, stroke_count->len);
    g_array_set_size (grade_level, grade_level->len);
    g_array_set_size (jlpt_level, jlpt_level->len);
    g_array_set_size (kun_readings, kun_readings->len);
    g_array_set_size (on_readings, on_readings->len);
    g_array_set_size (meanings, meanings->len);

    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_KANJI,
      (gchar**) g_array_free (kanji, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_UNICODE_SYMBOL, 
      (gchar**) g_array_free (unicode_symbol, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_USAGE_FREQUENCY,
      (gchar**) g_array_free (usage_frequency, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_STROKE_COUNT,
      (gchar**) g_array_free (stroke_count, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_GRADE_LEVEL,
      (gchar**) g_array_free (grade_level, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_JLPT_LEVEL,
      (gchar**) g_array_free (jlpt_level, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_KUN_READINGS,
      (gchar**) g_array_free (kun_readings, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_ON_READINGS, 
      (gchar**) g_array_free (on_readings, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_KANJIDICTIONARYCOLUMNID_MEANINGS,
      (gchar**) g_array_free (meanings, FALSE)
    );
}


static gint *
lw_kanjidictionary_calculate_applicable_columns_for_text (LwDictionary * dictionary,
                                                            gchar const  * TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_KANJIDICTIONARY (dictionary), NULL);
    if (TEXT == NULL || *TEXT == '\0') return NULL;

    //Declarations
    LwKanjiDictionary * self = NULL;
    LwDictionaryClass *klass = NULL;
    gint max_columns = 0;
    gint * columns = NULL;
    gint num_columns = 0;
    gboolean contains_kanji = FALSE;
    gboolean contains_furigana = FALSE;
    gboolean contains_romaji = FALSE;
    gboolean contains_number = FALSE;

    //Initializations
    self = LW_KANJIDICTIONARY (dictionary);
    max_columns = lw_kanjidictionary_get_total_columns (dictionary);
    columns = g_new0 (gint, max_columns + 1);
    contains_kanji = lw_utf8_contains_kanji (TEXT);
    contains_furigana = lw_utf8_contains_furigana (TEXT);
    contains_romaji = lw_utf8_contains_romaji (TEXT);
    contains_number = lw_utf8_contains_number (TEXT);

    g_assert_not_reached ();

    columns[num_columns++] = -1;

    if (num_columns < max_columns)
      columns = g_renew (gint, columns, num_columns);

errored:

    return columns;
}

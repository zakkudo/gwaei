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

/**
 * SECTION: exampledictionary
 * @short_description: Dictionary type for handling example formatted dictionary
 * @title: LwExampleDictionary
 *
 * A dictionary type for handling example formatted dictionaries.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib-object.h>

#include <libwaei/gettext.h>

#include <libwaei/dictionary.h>

#include <libwaei/dictionary/exampledictionary.h>

G_DEFINE_DYNAMIC_TYPE (LwExampleDictionary, lw_exampledictionary, LW_TYPE_DICTIONARY)

static gint lw_exampledictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_exampledictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_exampledictionary_get_column_handling (LwDictionary *self, gint column_num);
static gchar * lw_exampledictionary_columnize_line (LwDictionary *self, gchar *buffer, gchar **tokens, gsize *num_tokens);
static void lw_exampledictionary_load_columns (LwDictionary *self, gchar *buffer, gchar **tokens, gint num_tokens, LwParsedLine *line);
static gint * lw_exampledictionary_calculate_applicable_columns_for_text (LwDictionary * self, gchar const * TEXT);
static GType _columnid_type = 0;


static void 
lw_exampledictionary_init (LwExampleDictionary *exampledictionary)
{
}


static void
lw_exampledictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_exampledictionary_parent_class)->constructed (object);
    }

    LwDictionary *dictionary;
    gchar const * FILENAME = NULL;

    dictionary = LW_DICTIONARY (object);
    FILENAME = lw_dictionary_get_contents_filename (dictionary);

    gchar const * BUILT_IN_NAMES[] = {
      gettext("Examples"),
    };

    lw_dictionary_set_name (dictionary, gettext(FILENAME));
}


static void 
lw_exampledictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_exampledictionary_parent_class)->finalize (object);
}


static void
lw_exampledictionary_class_finalize (LwExampleDictionaryClass *klass)
{
}


gsize
lw_exampledictionary_count_lines (gchar      * contents,
                                  gsize        content_length,
                                  gsize      * max_line_length,
                                  LwProgress * progress)
{
    g_return_val_if_fail (contents != NULL, 0);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, 0);

    //Declarations    
    gint num_lines = 0;
    gsize max_chunk = 0;

    if (content_length < 1) content_length = strlen(contents);
    if (max_line_length != NULL) *max_line_length = 0;

    //Initializations
    max_chunk = LW_PROGRESS_START (progress, content_length);
    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "Delimiting lines...");
    }

    {
      gchar *c = NULL;
      gchar *p = NULL;
      gchar *e = NULL;
      gchar *n = NULL;

      gsize current = 0;
      gsize chunk = 0;
      gsize bytes_read = 0;

      n = p = c = contents;
      e = c + content_length;

      if (max_line_length != NULL)
      {
        *max_line_length = 0;
      }

      if (strncmp("A:", c, strlen("A:")) == 0)
      {
          num_lines++;
          c = g_utf8_next_char (c);
          c = g_utf8_next_char (c);
      }

      while (*c != '\0' && c < e)
      {
        if (strncmp("\nA:", c, strlen("\nA:")) == 0)
        {
          num_lines++;

          if (max_line_length != NULL)
          {
            gsize length = c - p;
            if (length > *max_line_length) *max_line_length = length;
          }

          n = lw_utf8_set_null_next_char (c);
          p = n;
          n = g_utf8_next_char (n);
          n = g_utf8_next_char (n);
        }
        else
        {
          n = g_utf8_next_char (c);
        }

        bytes_read = n - c;
        chunk += bytes_read;
        current += bytes_read;

        LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk);

        c = n;
      }

      LW_PROGRESS_FINISH (progress, current);

      if (max_line_length != NULL && *max_line_length == 0)
      {
        *max_line_length = c - contents;
      }
    }


errored:

    return num_lines;
}


static void
lw_exampledictionary_class_init (LwExampleDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_exampledictionary_finalize;
    object_class->constructed = lw_exampledictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->count_lines = lw_exampledictionary_count_lines;
    dictionary_class->get_column_handling = lw_exampledictionary_get_column_handling;
    dictionary_class->get_total_columns = lw_exampledictionary_get_total_columns;
    dictionary_class->get_column_language = lw_exampledictionary_get_column_language;
    dictionary_class->columnize_line = lw_exampledictionary_columnize_line;
    dictionary_class->load_columns = lw_exampledictionary_load_columns;
    dictionary_class->calculate_applicable_columns_for_text = lw_exampledictionary_calculate_applicable_columns_for_text;
    dictionary_class->get_columnid_type = lw_exampledictionary_get_columnid_type;
}


static gint
lw_exampledictionary_get_total_columns (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EXAMPLEDICTIONARY (self), 0);

    return TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS;
}



static gchar const *
lw_exampledictionary_get_column_language (LwDictionary *self,
                                    gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EXAMPLEDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gchar const * column_languages[TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_languages[LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE] = "ja";
      column_languages[LW_EXAMPLEDICTIONARYCOLUMNID_MEANING] = "en";
      column_languages[LW_EXAMPLEDICTIONARYCOLUMNID_ID] = "number";
    }

    return column_languages[column_num];
}


static LwDictionaryColumnHandling
lw_exampledictionary_get_column_handling (LwDictionary *self,
                                           gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EXAMPLEDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gint column_handlings[TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_handlings[LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_EXAMPLEDICTIONARYCOLUMNID_MEANING] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_EXAMPLEDICTIONARYCOLUMNID_ID] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
    }

    return column_handlings[column_num];
}


/**
 * lw_exampledictionary_get_columnid_type:
 * @Returns: The GType of the column id enumeration
 */
GType
lw_exampledictionary_get_columnid_type ()
{
    return _columnid_type;
}


static void
lw_exampledictionary_columnid_register_type (GTypeModule * module)
{
    GEnumValue static values[] = {
      { LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE, LW_EXAMPLEDICTIONARYCOLUMNNAME_PHRASE, LW_EXAMPLEDICTIONARYCOLUMNNICK_PHRASE },
      { LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE_WITH_READINGS, LW_EXAMPLEDICTIONARYCOLUMNNAME_PHRASE_WITH_READINGS, LW_EXAMPLEDICTIONARYCOLUMNNICK_PHRASE_WITH_READINGS },
      { LW_EXAMPLEDICTIONARYCOLUMNID_MEANING, LW_EXAMPLEDICTIONARYCOLUMNNAME_MEANING, LW_EXAMPLEDICTIONARYCOLUMNNICK_MEANING },
      { LW_EXAMPLEDICTIONARYCOLUMNID_ID, LW_EXAMPLEDICTIONARYCOLUMNNAME_ID, LW_EXAMPLEDICTIONARYCOLUMNNICK_ID },
      { 0, NULL, NULL },
    };

    _columnid_type = g_type_module_register_enum (module, "LwExampleDictionaryColumnId", values);
}

/**
 * register_dictionary_module_type:
 * @module: The module to regester the type using
 */
G_MODULE_EXPORT void
register_dictionary_module_type (GTypeModule * module)
{
  lw_exampledictionary_register_type (module);
  lw_exampledictionary_columnid_register_type (module);
}


/**
 * lw_exampledictionary_tokenize:
 * @buffer The text to tokenize.  It is tokenized in place and no copy is made.
 * @tokens A pointer to an alloced array to place the tokens.  This array
 * should have enough space to hold the tokenized buffer positions which is usually
 * (strlen(@buffer) + 1) * sizeof(gchar*).  The token array is %NULL terminated.
 * @num_tokens The number of tokens that were created in @tokens
 *
 * Columnizes a string given the standards of edict dictionaries by placing %NULL
 * characters in the buffer, and recording the positions in the @tokens array.
 * This method is made to token one line at a time.
 *
 * Returns: The end of the filled token array
 */
static gchar *
lw_exampledictionary_columnize_line (LwDictionary  * self,
                                     gchar         * buffer,
                                     gchar        ** tokens,
                                     gsize         * num_tokens)
{
    //Sanity checks
    g_return_if_fail (LW_IS_EXAMPLEDICTIONARY (self));
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (tokens != NULL, NULL);

    //Declarations
    gchar *c = NULL;
    gint length = 0;

    //Initializations
    c = buffer;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //^
    //HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}
    
    if (strncmp("A:", c, strlen("A:")) != 0) goto errored;

    tokens[length++] = c;

    c += strlen("A:");

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //   ^
    //   HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}

    while (*c != '\t') c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //                    ^
    //                    HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}

    tokens[length++] = c;

    while (*c != '\0' && strncmp(c, "#ID=", strlen("#ID=")) != 0) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    tokens[length++] = c;

    c += strlen("ID=");

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //                                            ^
    //                                            HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}

    while (g_ascii_isdigit (*c) || g_ascii_ispunct (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = lw_utf8_set_null_next_char (c);

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //                                            ^
    //                                            HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}
    
    while (*c != '\0' && strncmp(c, "B:", strlen("B:")) != 0) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    while (g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    tokens[length++] = c;

    while (*c != '\0' && *c != '\n') c = g_utf8_next_char (c);
    if (*c == '\n') c = lw_utf8_set_null_next_char (c);

errored:

    tokens[length] = NULL;

    if (num_tokens != NULL)
    {
      *num_tokens = length;
    }

    return c;
}


static void
lw_exampledictionary_load_columns (LwDictionary  *self,
                                   gchar         *buffer,
                                   gchar        **tokens,
                                   gint           num_tokens,
                                   LwParsedLine  *line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_EXAMPLEDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (tokens != NULL);
    g_return_if_fail (num_tokens > 0);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *phrase = NULL;
    GArray *meaning = NULL;
    GArray *id = NULL;
    GArray *phrase_with_readings = NULL;


    //Initializations
    phrase = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);
    meaning = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);
    id = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);
    phrase_with_readings = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);

    {
      gint i = 0;

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //   ^
      //   HERE
      if (tokens[i] == NULL || i >= num_tokens) goto errored;
      if (strncmp(tokens[i], "A: ", strlen("A: ")) != 0) goto errored;
      tokens[i] += strlen("A: ");
      g_array_append_val (phrase, tokens[i]);
      i++;

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //                    ^
      //                    HERE
      if (tokens[i] == NULL || i >= num_tokens) goto errored;
      g_array_append_val (meaning, tokens[i]);
      i++;

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //                                         ^
      //                                         HERE
      if (tokens[i] == NULL || i >= num_tokens) goto errored;
      if (strncmp(tokens[i], "ID=", strlen("ID=")) != 0) goto errored;
      tokens[i] += strlen("ID=");
      g_array_append_val (id, tokens[i]);
      i++;

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //^
      //HERE
      if (tokens[i] == NULL || i >= num_tokens)
      if (strncmp(tokens[i], "B: ", strlen("B: ")) != 0) goto errored;
      tokens[i] += strlen("B: ");
      g_array_append_val (phrase_with_readings, tokens[i]);
      i++;
    }

errored:

    {
      GArray * columns[TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS] = {0};
      gint i = 0;

      columns[LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE] = phrase;
      columns[LW_EXAMPLEDICTIONARYCOLUMNID_MEANING] = meaning;
      columns[LW_EXAMPLEDICTIONARYCOLUMNID_ID] = id;
      columns[LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE_WITH_READINGS] = phrase_with_readings;

      for (i = 0; i < TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS; i++)
      {
        if (columns[i] != NULL && columns[i]->len > 0)
        {
          g_array_set_size (columns[i], columns[i]->len);
          lw_parsedline_set_strv (line, i, (gchar**) g_array_free (columns[i], FALSE));
        }
        else
        {
          g_array_free (columns[i], TRUE);
        }
      }
    }
}


static gint *
lw_exampledictionary_calculate_applicable_columns_for_text (LwDictionary * dictionary,
                                                            gchar const  * TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EXAMPLEDICTIONARY (dictionary), NULL);
    if (TEXT == NULL || *TEXT == '\0') return NULL;

    //Declarations
    LwExampleDictionary * self = NULL;
    LwDictionaryClass *klass = NULL;
    gint max_columns = 0;
    gint * columns = NULL;
    gint num_columns = 0;
    gboolean contains_kanji = FALSE;
    gboolean contains_furigana = FALSE;
    gboolean contains_romaji = FALSE;
    gboolean contains_number = FALSE;

    //Initializations
    self = LW_EXAMPLEDICTIONARY (dictionary);
    max_columns = lw_exampledictionary_get_total_columns (dictionary);
    columns = g_new0 (gint, max_columns + 1);
    contains_kanji = lw_utf8_contains_kanji (TEXT);
    contains_furigana = lw_utf8_contains_furigana (TEXT);
    contains_romaji = lw_utf8_contains_romaji (TEXT);
    contains_number = lw_utf8_contains_number (TEXT);

    if (!contains_kanji && !contains_furigana && contains_romaji)
    {
      columns[num_columns++] = LW_EXAMPLEDICTIONARYCOLUMNID_MEANING;
    }

    if (contains_kanji || contains_furigana)
    {
      columns[num_columns++] = LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE;
    }

    if (!contains_kanji && !contains_furigana && contains_number)
    {
      columns[num_columns++] = LW_EXAMPLEDICTIONARYCOLUMNID_ID;
    }

    columns[num_columns++] = -1;

    if (num_columns < max_columns)
      columns = g_renew (gint, columns, num_columns);

errored:

    return columns;
}

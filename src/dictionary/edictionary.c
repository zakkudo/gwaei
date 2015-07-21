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
 * SECTION: edictionary
 * @short_description: Dictionary type for handling edict formatted dictionary
 * @title: LwEDictionary
 *
 * A dictionary type for handling edict formatted dictionaries.
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

#include <libwaei/dictionary/edictionary.h>

G_DEFINE_DYNAMIC_TYPE (LwEDictionary, lw_edictionary, LW_TYPE_DICTIONARY)

static gint lw_edictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_edictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_edictionary_get_column_handling (LwDictionary *self, gint column_num);
static gchar * lw_edictionary_columnize_line (LwDictionary *self, gchar *buffer, gchar **tokens, gsize *num_tokens);
static void lw_edictionary_load_columns (LwDictionary *self, gchar *buffer, gchar **tokens, gint num_tokens, LwParsedLine *line);
static gint * lw_edictionary_calculate_applicable_columns_for_text (LwDictionary * self, gchar const * TEXT);
static GType _columnid_type = 0;


static void 
lw_edictionary_init (LwEDictionary *edictionary)
{
}


static void
lw_edictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_edictionary_parent_class)->constructed (object);
    }

    LwDictionary *dictionary;
    gchar const * FILENAME = NULL;

    gchar const *BUILD_IN_NAMES[] = {
      gettext("English"),
      gettext("Names"),
      gettext("Places"),
      gettext("Names and Places"),
    };

    dictionary = LW_DICTIONARY (object);
    FILENAME = lw_dictionary_get_contents_filename (dictionary);

    lw_dictionary_set_name (dictionary, gettext(FILENAME));
}


static void 
lw_edictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_edictionary_parent_class)->finalize (object);
}


static void
lw_edictionary_class_finalize (LwEDictionaryClass *klass)
{
}


static void
lw_edictionary_class_init (LwEDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_edictionary_finalize;
    object_class->constructed = lw_edictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->count_lines = lw_utf8_replace_linebreaks_with_nullcharacter;
    dictionary_class->get_column_handling = lw_edictionary_get_column_handling;
    dictionary_class->get_total_columns = lw_edictionary_get_total_columns;
    dictionary_class->get_column_language = lw_edictionary_get_column_language;
    dictionary_class->columnize_line = lw_edictionary_columnize_line;
    dictionary_class->load_columns = lw_edictionary_load_columns;
    dictionary_class->calculate_applicable_columns_for_text = lw_edictionary_calculate_applicable_columns_for_text;
    dictionary_class->get_columnid_type = lw_edictionary_get_columnid_type;
}


static gint
lw_edictionary_get_total_columns (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EDICTIONARY (self), 0);

    return TOTAL_LW_EDICTIONARYCOLUMNIDS;
}



static gchar const *
lw_edictionary_get_column_language (LwDictionary *self,
                                    gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_EDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gchar const * column_languages[TOTAL_LW_EDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_languages[LW_EDICTIONARYCOLUMNID_WORD] = "ja";
      column_languages[LW_EDICTIONARYCOLUMNID_READING] = "ja";
      column_languages[LW_EDICTIONARYCOLUMNID_DEFINITION] = "en";
    }

    return column_languages[column_num];
}


static LwDictionaryColumnHandling
lw_edictionary_get_column_handling (LwDictionary *self,
                                    gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_EDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gint column_handlings[TOTAL_LW_EDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_handlings[LW_EDICTIONARYCOLUMNID_WORD] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_EDICTIONARYCOLUMNID_READING] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_EDICTIONARYCOLUMNID_DEFINITION] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_EDICTIONARYCOLUMNID_CLASSIFICATION] = LW_DICTIONARYCOLUMNHANDLING_FILTER_ONLY;
      column_handlings[LW_EDICTIONARYCOLUMNID_POPULAR] = LW_DICTIONARYCOLUMNHANDLING_FILTER_ONLY;
    }

    return column_handlings[column_num];
}


/**
 * lw_edictionary_get_columnid_type:
 * @Returns: The GType of the column id enumeration
 */
GType
lw_edictionary_get_columnid_type ()
{
    return _columnid_type;
}


static void
lw_edictionary_columnid_register_type (GTypeModule * module)
{
    static GEnumValue values[] = {
      { LW_EDICTIONARYCOLUMNID_WORD, LW_EDICTIONARYCOLUMNNAME_WORD, LW_EDICTIONARYCOLUMNNICK_WORD },
      { LW_EDICTIONARYCOLUMNID_READING, LW_EDICTIONARYCOLUMNNAME_READING, LW_EDICTIONARYCOLUMNNICK_READING },
      { LW_EDICTIONARYCOLUMNID_DEFINITION, LW_EDICTIONARYCOLUMNNAME_DEFINITION, LW_EDICTIONARYCOLUMNNICK_DEFINITION },
      { LW_EDICTIONARYCOLUMNID_CLASSIFICATION, LW_EDICTIONARYCOLUMNNAME_CLASSIFICATION, LW_EDICTIONARYCOLUMNNICK_CLASSIFICATION },
      { LW_EDICTIONARYCOLUMNID_POPULAR, LW_EDICTIONARYCOLUMNNAME_POPULAR, LW_EDICTIONARYCOLUMNNICK_POPULAR },
      { 0, NULL, NULL },
    };

    _columnid_type = g_type_module_register_enum (module, "LwEDictitionaryColumnId", values);
}


G_MODULE_EXPORT void
register_dictionary_module_type (GTypeModule * module)
{
  lw_edictionary_register_type (module);
  lw_edictionary_columnid_register_type (module);
}


/**
 * lw_edictionary_columnize:
 * @buffer: (transfer none): The text to columnize.  It is columnized in place and no copy is made.
 * @tokens: (out) (transfer-full):A pointer to an alloced array to place the tokens.  This array
 * should have enough space to hold the tokenized buffer positions which is usually
 * (strlen(@buffer) + 1) * sizeof(gchar*).  The token array is %NULL terminated.
 * @num_tokens: (out): The number of tokens that were created in @tokens
 *
 * Columnizes a string given the standards of edict dictionaries by placing %NULL
 * characters in the buffer, and recording the positions in the @tokens array.
 * This method is made to tokenize one line at a time.
 *
 * Returns: The end of the line
 */
static gchar *
lw_edictionary_columnize_line (LwDictionary  * self,
                               gchar         * buffer,
                               gchar        ** tokens,
                               gsize         * num_tokens)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EDICTIONARY (self), NULL);
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (tokens != NULL, NULL);

    //Declarations
    gchar *c = NULL;
    gint length = 0;

    //Initializations
    c = buffer;

    while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //^
    //HERE

    tokens[length++] = c;

    while (*c != '\0' && !g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //     ^
    //     HERE

    if (*c == '[')
    {
      c = g_utf8_next_char (c);
      gchar* e = c;
      while (*e !='\0' && *e !=']' && *e != '/') e = g_utf8_next_char (e);
      if (*e == '\0') goto errored;

      if (*e == ']' && e - c > 1)
      {
        tokens[length++] = c;
        c = lw_utf8_set_null_next_char (e);
      }
      else
      {
        c = e;
      }
      while (*c != '\0' && !g_ascii_isspace (*c)) c = g_utf8_next_char (c);
      if (*c == '\0') goto errored;
    }

    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //                ^
    //                HERE

    while (*c != '\0' && *c != '/') c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = g_utf8_next_char (c);
    if (*c == '\0' || *c != '(') goto errored;
    
    c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    {
      gchar *e = c;
      while (*e != '\0' && *e != ')') e = g_utf8_next_char (e);
      if (*e == '\0') goto errored;

      if (e - c > 1) tokens[length++] = c;

      c = lw_utf8_set_null_next_char (e);
    }

    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //                     ^
    //                     HERE

    while (*c != '\0')
    {
      while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c);

      //Skip over the digit portion
      if (*c == '(' && g_ascii_isdigit(*(c + 1)))
      {
        c = g_utf8_next_char (c);
        while (*c != '\0' && (*c == ')' || g_ascii_isdigit(*c) || g_ascii_isspace(*c))) c = g_utf8_next_char (c);
        if (*c == '\0') goto errored;;
      }

      {
        gchar *e = c;
        while (*e != '\0' && *e != '/') e = g_utf8_next_char (e);
        if (*e == '\0') goto errored;;

        if (e - c > 1) tokens[length++] = c;

        c = lw_utf8_set_null_next_char (e);
      }
    }

errored:

    tokens[length] = NULL;

    if (num_tokens != NULL)
    {
      *num_tokens = length;
    }

    return c;
}


static void
lw_edictionary_load_columns (LwDictionary  * self,
                             gchar         * buffer,
                             gchar        ** tokens,
                             gint            num_tokens,
                             LwParsedLine  * line) 
{
    //Sanity checks
    g_return_if_fail (LW_IS_EDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (tokens != NULL);
    g_return_if_fail (num_tokens > 0);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *word = NULL;
    GArray *reading = NULL;
    GArray *definition = NULL;
    GArray *classification = NULL;
    GArray *popular = NULL;

    //Initializations
    word = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    reading = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    definition = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    classification = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);
    popular = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);

    { //Get the element at the end first so the forward iteration is simpler...
      gint i = num_tokens - 1;

      if (tokens[i] != NULL && strcmp (tokens[i], "(P)") == 0)
      {
        g_array_append_val (popular, tokens[i]);
        i--;
      }

      num_tokens = i + 1;

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //                                                                                             ^
      //                                                                                             HERE
    }

    {
      gint i = 0;

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //^
      //HERE

      if (tokens[i] != NULL && i < num_tokens)
      {
        g_array_append_val (word, tokens[i]);
        i++;
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //      ^
      //      HERE

      {
        GUnicodeScript script = g_unichar_get_script (g_utf8_get_char (tokens[i]));
        if (tokens[i] != NULL && i < num_tokens && script == G_UNICODE_SCRIPT_HIRAGANA || script == G_UNICODE_SCRIPT_KATAKANA)
        {
          g_array_append_val (reading, tokens[i]);
          i++;
        }
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //                 ^
      //                 HERE

      {
        if (tokens[i] != NULL && i < num_tokens)
        {
          gchar *c = tokens[i];
          while (*c != '\0')
          {
            g_array_append_val (classification, c);
            while (*c != '\0' && *c != ',') c = g_utf8_next_char (c);
            if (*c == ',') c = lw_utf8_set_null_next_char (c);
          }
          i++;
        }
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //                         ^
      //                         HERE

      while (tokens[i] != NULL & i < num_tokens)
      {
        g_array_append_val (definition, tokens[i]);
        i++;
      }
    }

errored:

    {
      GArray * columns[TOTAL_LW_EDICTIONARYCOLUMNIDS] = {0};
      gint i = 0;

      columns[LW_EDICTIONARYCOLUMNID_POPULAR] = popular;
      columns[LW_EDICTIONARYCOLUMNID_WORD] = word;
      columns[LW_EDICTIONARYCOLUMNID_READING] = reading;
      columns[LW_EDICTIONARYCOLUMNID_DEFINITION] = definition;
      columns[LW_EDICTIONARYCOLUMNID_CLASSIFICATION] = classification;

      for (i = 0; i < TOTAL_LW_EDICTIONARYCOLUMNIDS; i++)
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
lw_edictionary_calculate_applicable_columns_for_text (LwDictionary * dictionary,
                                                      gchar const  * TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EDICTIONARY (dictionary), NULL);
    if (TEXT == NULL || *TEXT == '\0') return NULL;

    //Declarations
    LwEDictionary * self = NULL;
    LwDictionaryClass *klass = NULL;
    gint max_columns = 0;
    gint * columns = NULL;
    gint num_columns = 0;
    gboolean contains_kanji = FALSE;
    gboolean contains_furigana = FALSE;
    gboolean contains_romaji = FALSE;
    gboolean contains_number = FALSE;

    //Initializations
    self = LW_EDICTIONARY (dictionary);
    max_columns = lw_edictionary_get_total_columns (dictionary);
    columns = g_new0 (gint, max_columns + 1);
    contains_kanji = lw_utf8_contains_kanji (TEXT);
    contains_furigana = lw_utf8_contains_furigana (TEXT);
    contains_romaji = lw_utf8_contains_romaji (TEXT);
    contains_number = lw_utf8_contains_number (TEXT);

    if (contains_kanji || (contains_kanji && contains_furigana))
    {
      columns[num_columns++] = LW_EDICTIONARYCOLUMNID_WORD;
    }
    if (contains_furigana && !contains_kanji)
    {
      columns[num_columns++] = LW_EDICTIONARYCOLUMNID_READING;
    }
    if (contains_romaji || contains_number)
    {
      columns[num_columns++] = LW_EDICTIONARYCOLUMNID_DEFINITION;
    }

    columns[num_columns++] = -1;

    if (num_columns < max_columns)
    {
      columns = g_renew (gint, columns, num_columns);
    }

errored:

    return columns;
}

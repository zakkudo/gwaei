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
//!  @file radicalsdictionary.c
//!
//!  @brief LwDictionary objects represent a loaded dictionary that the program
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

#include <libwaei/dictionary.h>

#include <libwaei/dictionary/radicalsdictionary.h>


static gint lw_radicalsdictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_radicalsdictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_radicalsdictionary_get_column_handling (LwDictionary *self, gint column_num);
static gchar * lw_radicalsdictionary_columnize_line (LwDictionary *self, gchar *buffer, gchar **tokens, gsize *num_tokens);
static void lw_radicalsdictionary_load_columns (LwDictionary *self, gchar *buffer, gchar **tokens, gint num_tokens, LwParsedLine *line);
static gint * lw_radicalsdictionary_calculate_applicable_columns_for_text (LwDictionary * self, gchar const * TEXT);

G_DEFINE_DYNAMIC_TYPE (LwRadicalsDictionary, lw_radicalsdictionary, LW_TYPE_DICTIONARY)


static void 
lw_radicalsdictionary_init (LwRadicalsDictionary *radicalsdictionary)
{
}


static void
lw_radicalsdictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_radicalsdictionary_parent_class)->constructed (object);
    }

    LwDictionary *dictionary;
    gchar const * FILENAME = NULL;

    dictionary = LW_DICTIONARY (object);
    FILENAME = lw_dictionary_get_contents_filename (dictionary);

    gchar const * BUILT_IN_NAMES[] = {
      gettext("Radicals"),
    };

    lw_dictionary_set_name (dictionary, gettext(FILENAME));
}


static void 
lw_radicalsdictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_radicalsdictionary_parent_class)->finalize (object);
}


static void
lw_radicalsdictionary_class_finalize (LwRadicalsDictionaryClass *klass)
{
}


static void
lw_radicalsdictionary_class_init (LwRadicalsDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_radicalsdictionary_finalize;
    object_class->constructed = lw_radicalsdictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->get_column_handling = lw_radicalsdictionary_get_column_handling;
    dictionary_class->get_total_columns = lw_radicalsdictionary_get_total_columns;
    dictionary_class->get_column_language = lw_radicalsdictionary_get_column_language;
    dictionary_class->columnize_line = lw_radicalsdictionary_columnize_line;
    dictionary_class->load_columns = lw_radicalsdictionary_load_columns;
    dictionary_class->calculate_applicable_columns_for_text = lw_radicalsdictionary_calculate_applicable_columns_for_text;
    dictionary_class->columnid_get_type = lw_radicalsdictionary_columnid_get_type;
}


static gint
lw_radicalsdictionary_get_total_columns (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RADICALSDICTIONARY (self), 0);

    return TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS;
}



static gchar const *
lw_radicalsdictionary_get_column_language (LwDictionary *self,
                                    gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RADICALSDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gchar const * column_languages[TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_languages[LW_RADICALSDICTIONARYCOLUMNID_KANJI] = "ja";
      column_languages[LW_RADICALSDICTIONARYCOLUMNID_RADICALS] = "ja";
    }

    return column_languages[column_num];
}


static LwDictionaryColumnHandling
lw_radicalsdictionary_get_column_handling (LwDictionary *self,
                                           gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RADICALSDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gint column_handlings[TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_handlings[LW_RADICALSDICTIONARYCOLUMNID_KANJI] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
      column_handlings[LW_RADICALSDICTIONARYCOLUMNID_RADICALS] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
    }

    return column_handlings[column_num];
}


GType
lw_radicalsdictionary_columnid_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_RADICALSDICTIONARYCOLUMNID_KANJI, LW_RADICALSDICTIONARYCOLUMNNAME_KANJI, LW_RADICALSDICTIONARYCOLUMNNICK_KANJI },
        { LW_RADICALSDICTIONARYCOLUMNID_RADICALS, LW_RADICALSDICTIONARYCOLUMNNAME_RADICALS, LW_RADICALSDICTIONARYCOLUMNNICK_RADICALS },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwRadicalsDictionaryColumnId", values);
    }

    return type;
}


G_MODULE_EXPORT void
register_dictionary_module_type (GTypeModule * module)
{
  lw_radicalsdictionary_register_type (module);
}


/**
 * lw_edictionary_tokenize
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
lw_radicalsdictionary_columnize_line (LwDictionary  *self,
                                      gchar         *buffer,
                                      gchar        **tokens,
                                      gsize         *num_tokens)
{
    //Sanity checks
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (tokens != NULL, NULL);

    //Declarations
    gchar *c = NULL;
    gint length = 0;

    //Initializations
    c = buffer;

    {
      gchar *c = buffer;
      if (c == NULL || *c == '\0') goto errored;

      //An example line
      //鯵 : 魚 大 田 厶 彡 杰
      //^
      //HERE

      tokens[length++] = c;
      while (*c != '\0' && !g_ascii_isspace(*c) && !g_ascii_ispunct (*c)) c = g_utf8_next_char (c);
      if (*c == '\0') goto errored;

      //An example line
      //鯵 : 魚 大 田 厶 彡 杰
      //  ^
      //  HERE

      c = lw_utf8_set_null_next_char (c);

      while (*c != '\0' && (g_ascii_isspace (*c) || g_ascii_ispunct (*c))) c = g_utf8_next_char (c); 
      if (*c == '\0') goto errored;

      //An example line
      //鯵 : 魚 大 田 厶 彡 杰
      //     ^
      //     HERE

      while (*c != '\0')
      {
        tokens[length++] = c;

        while (*c != '\0' && !g_ascii_isspace (*c) && !g_ascii_ispunct (*c)) c = g_utf8_next_char (c);
        if (*c == '\0') goto errored;

        c = lw_utf8_set_null_next_char (c);
        if (*c == '\0') goto errored;

        while (*c != '\0' && (g_ascii_isspace (*c) || g_ascii_ispunct (*c))) c = g_utf8_next_char (c); 
        if (*c == '\0') goto errored;
      }
    }

errored:

    tokens[length] = NULL;

    return c;
}


static void
lw_radicalsdictionary_load_columns (LwDictionary  *self,
                                    gchar         *buffer,
                                    gchar        **tokens,
                                    gint           num_tokens,
                                    LwParsedLine  *line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_RADICALSDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (tokens != NULL);
    g_return_if_fail (num_tokens > 0);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *kanji = NULL;
    GArray *radicals = NULL;
    gint i = 0;

    //Initializations
    kanji = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);
    radicals = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_tokens);

    if (i < num_tokens)
    {
      g_array_append_val (kanji, tokens[i++]);
    }

    while (i < num_tokens)
    {
      g_array_append_val (radicals, tokens[i++]);
    }

errored:

    g_array_set_size (kanji, 1);
    g_array_set_size (radicals, radicals->len);

    lw_parsedline_set_strv (
      line,
      LW_RADICALSDICTIONARYCOLUMNID_KANJI,
      (gchar**) g_array_free (kanji, FALSE)
    );

    lw_parsedline_set_strv (
      line,
      LW_RADICALSDICTIONARYCOLUMNID_RADICALS,
      (gchar**) g_array_free (radicals, FALSE)
    );
}


static gint *
lw_radicalsdictionary_calculate_applicable_columns_for_text (LwDictionary * dictionary,
                                                            gchar const  * TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RADICALSDICTIONARY (dictionary), NULL);
    if (TEXT == NULL || *TEXT == '\0') return NULL;

    //Declarations
    LwRadicalsDictionary * self = NULL;
    LwDictionaryClass *klass = NULL;
    gint max_columns = 0;
    gint * columns = NULL;
    gint num_columns = 0;
    gboolean contains_radicals = FALSE;
    gboolean contains_kanji = FALSE;
    gboolean contains_furigana = FALSE;
    gboolean contains_romaji = FALSE;
    gboolean contains_number = FALSE;

    //Initializations
    self = LW_RADICALSDICTIONARY (dictionary);
    max_columns = lw_radicalsdictionary_get_total_columns (dictionary);
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

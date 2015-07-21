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
 * SECTION: unknowndictionary
 * @short_description: Dictionary type for handling unknown dictionary formats
 * @title: LwUnknownDictionary
 *
 * A dictionary type for handling unknown dictionary formats
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

#include <libwaei/dictionary/unknowndictionary.h>


G_DEFINE_DYNAMIC_TYPE (LwUnknownDictionary, lw_unknowndictionary, LW_TYPE_DICTIONARY)

static gint lw_unknowndictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_unknowndictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_unknowndictionary_get_column_handling (LwDictionary *self, gint column_num);
static gchar * lw_unknowndictionary_columnize_line (LwDictionary *self, gchar *buffer, gchar **tokens, gsize *num_tokens);
static void lw_unknowndictionary_load_columns (LwDictionary *self, gchar *buffer, gchar **tokens, gint num_tokens, LwParsedLine *line);
static gint * lw_unknowndictionary_calculate_applicable_columns_for_text (LwDictionary * self, gchar const * TEXT);
static GType _columnid_type = 0;


static void 
lw_unknowndictionary_init (LwUnknownDictionary *unknowndictionary)
{
}


static void
lw_unknowndictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_unknowndictionary_parent_class)->constructed (object);
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
lw_unknowndictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_unknowndictionary_parent_class)->finalize (object);
}


static void
lw_unknowndictionary_class_finalize (LwUnknownDictionaryClass *klass)
{
}


static void
lw_unknowndictionary_class_init (LwUnknownDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_unknowndictionary_finalize;
    object_class->constructed = lw_unknowndictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->count_lines = lw_utf8_replace_linebreaks_with_nullcharacter;
    dictionary_class->get_column_handling = lw_unknowndictionary_get_column_handling;
    dictionary_class->get_total_columns = lw_unknowndictionary_get_total_columns;
    dictionary_class->get_column_language = lw_unknowndictionary_get_column_language;
    dictionary_class->columnize_line = lw_unknowndictionary_columnize_line;
    dictionary_class->load_columns = lw_unknowndictionary_load_columns;
    dictionary_class->calculate_applicable_columns_for_text = lw_unknowndictionary_calculate_applicable_columns_for_text;
    dictionary_class->columnid_get_type = lw_unknowndictionary_columnid_get_type;
}


static gint
lw_unknowndictionary_get_total_columns (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_UNKNOWNDICTIONARY (self), 0);

    return TOTAL_LW_UNKNOWNDICTIONARYCOLUMNIDS;
}



static gchar const *
lw_unknowndictionary_get_column_language (LwDictionary *self,
                                          gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_UNKNOWNDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_UNKNOWNDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gchar const * column_languages[TOTAL_LW_UNKNOWNDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_languages[LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN] = "unknown";
    }

    return column_languages[column_num];
}


static LwDictionaryColumnHandling
lw_unknowndictionary_get_column_handling (LwDictionary *self,
                                          gint          column_num)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_UNKNOWNDICTIONARY (self), 0);
    g_return_val_if_fail (column_num > -1, 0);
    g_return_val_if_fail (column_num < TOTAL_LW_UNKNOWNDICTIONARYCOLUMNIDS, 0);

    static gboolean initialized = FALSE;
    static gint column_handlings[TOTAL_LW_UNKNOWNDICTIONARYCOLUMNIDS] = {0};

    if (G_UNLIKELY (initialized == FALSE))
    {
      initialized = TRUE;
      column_handlings[LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN] = LW_DICTIONARYCOLUMNHANDLING_SEARCH_ONLY;
    }

    return column_handlings[column_num];
}


/**
 * lw_unknowndictionary_columnid_get_type:
 * @Returns: The GType of the column id enumeration
 */
GType
lw_unknowndictionary_columnid_get_type ()
{
    return _columnid_type;
}



/**
 * lw_unknowndictionary_columnid_get_type:
 * @Returns: The GType of the column id enumeration
 */
static void
lw_unknowndictionary_columnid_register_type (GTypeModule * module)
{
    GEnumValue values[] = {
      { LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN, LW_UNKNOWNDICTIONARYCOLUMNNAME_UNKNOWN, LW_UNKNOWNDICTIONARYCOLUMNNICK_UNKNOWN },
      { 0, NULL, NULL },
    };

    _columnid_type = g_type_module_register_enum (module, "LwUnknownDictionaryColumnId", values);
}


G_MODULE_EXPORT void
register_dictionary_module_type (GTypeModule * module)
{
  lw_unknowndictionary_register_type (module);
  lw_unknowndictionary_columnid_register_type (module);
}


/**
 * lw_unknowndictionary_tokenize
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
lw_unknowndictionary_columnize_line (LwDictionary  *self,
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

    tokens[length++] = c;

    tokens[length] = NULL;

    if (num_tokens != NULL)
    {
      *num_tokens = length;
    }

    return c;
}


static void
lw_unknowndictionary_load_columns (LwDictionary  *self,
                                   gchar         *buffer,
                                   gchar        **tokens,
                                   gint           num_tokens,
                                   LwParsedLine  *line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_UNKNOWNDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (tokens != NULL);
    g_return_if_fail (num_tokens > 0);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *unknown = NULL;

    //Initializations
    unknown = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);

    {
      gint i = 0;

      if (i < num_tokens)
      {
        g_array_append_val (unknown, tokens[i]);
      }
    }

errored:

    g_array_set_size (unknown, unknown->len);

    lw_parsedline_set_strv (
      line,
      LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN,
      (gchar**) g_array_free (unknown, FALSE)
    );
}


static gint *
lw_unknowndictionary_calculate_applicable_columns_for_text (LwDictionary * dictionary,
                                                            gchar const  * TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_UNKNOWNDICTIONARY (dictionary), NULL);
    if (TEXT == NULL || *TEXT == '\0') return NULL;

    //Declarations
    LwUnknownDictionary * self = NULL;
    LwDictionaryClass *klass = NULL;
    gint max_columns = 0;
    gint * columns = NULL;
    gint num_columns = 0;
    gboolean contains_kanji = FALSE;
    gboolean contains_furigana = FALSE;
    gboolean contains_romaji = FALSE;
    gboolean contains_number = FALSE;

    //Initializations
    self = LW_UNKNOWNDICTIONARY (dictionary);
    max_columns = lw_unknowndictionary_get_total_columns (dictionary);
    columns = g_new0 (gint, max_columns + 1);
    contains_kanji = lw_utf8_contains_kanji (TEXT);
    contains_furigana = lw_utf8_contains_furigana (TEXT);
    contains_romaji = lw_utf8_contains_romaji (TEXT);
    contains_number = lw_utf8_contains_number (TEXT);

    columns[num_columns++] = LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN;
    columns[num_columns++] = -1;

    if (num_columns < max_columns)
      columns = g_renew (gint, columns, num_columns);

errored:

    return columns;
}

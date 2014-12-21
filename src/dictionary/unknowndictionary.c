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
//!  @file unknowndictionary.c
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

#include <libwaei/libwaei.h>
#include <libwaei/dictionary-private.h>
#include <libwaei/gettext.h>

#include <dictionary/unknowndictionary.h>


G_DEFINE_TYPE (LwUnknownDictionary, lw_unknowndictionary, LW_TYPE_DICTIONARY)

static LwParsed* lw_unknowndictionary_parse (LwUnknownDictionary *self, gchar *contents, gsize content_length, LwProgress *progress);
static gint lw_unknowndictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_unknowndictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_unknowndictionary_get_column_handling (LwDictionary *self, gint column_num);

LwDictionary* lw_unknowndictionary_new (const gchar        *FILENAME,
                                        LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_UNKNOWNDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                 NULL));

    return dictionary;
}


static void 
lw_unknowndictionary_init (LwUnknownDictionary *dictionary)
{
}


static void
lw_unknowndictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_unknowndictionary_parent_class)->constructed (object);
    }

/*
    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;
*/
}


static void 
lw_unknowndictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_unknowndictionary_parent_class)->finalize (object);
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
    dictionary_class->priv->parse = (LwDictionaryParseFunc) lw_unknowndictionary_parse;
    dictionary_class->priv->get_column_handling = lw_unknowndictionary_get_column_handling;
    dictionary_class->priv->get_total_columns = lw_unknowndictionary_get_total_columns;
    dictionary_class->priv->get_column_language = lw_unknowndictionary_get_column_language;
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
      column_handlings[LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN] = LW_DICTIONARYCOLUMNHANDLING_INDEX_AND_SEARCH;
    }

    return column_handlings[column_num];
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
static gchar**
lw_unknowndictionary_tokenize_line (LwUnknownDictionary  *self,
                                    gchar                *buffer,
                                    gchar               **tokens,
                                    gsize                *num_tokens)
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

    return tokens + length;
}


static void
lw_unknowndictionary_load_line_tokens (LwUnknownDictionary  *self,
                                       gchar                *buffer,
                                       gchar               **tokens,
                                       gint                  num_tokens,
                                       LwParsedLine         *line)
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

    lw_parsedline_take_strv (
      line,
      LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN,
      (gchar**) g_array_free (unknown, FALSE)
    );
}


//!
//! @brief Parses a string for an unknown format string
//! @param rl The Resultline object this method works on
//!
static LwParsed*
lw_unknowndictionary_parse (LwUnknownDictionary *self,
                            gchar               *contents,
                            gsize                content_length,
                            LwProgress          *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_UNKNOWNDICTIONARY (self), NULL);
    g_return_val_if_fail (contents != NULL, NULL);

    //Declarations
    gint num_lines = 0;
    gchar **tokens = NULL;
    gsize max_line_length = 0;
    gsize num_tokens = 0;
    gint length = -1;
    LwParsed *parsed = NULL;
    LwParsedLine* lines = NULL;

    //Initializations
    if (content_length < 1) content_length = strlen(contents);
    num_lines = lw_utf8_replace_linebreaks_with_nullcharacter (contents, content_length, &max_line_length, progress);
    if (num_lines < 1) goto errored;
    if (max_line_length < 1) goto errored;
    parsed = lw_parsed_new (contents, content_length);
    if (parsed == NULL) goto errored;
    lines = g_new0 (LwParsedLine, num_lines);
    if (lines == NULL) goto errored;
    tokens = g_new0 (gchar*, max_line_length + 1);
    if (tokens == NULL) goto errored;

    if (progress != NULL)
    {
      lw_progress_set_secondary_message (progress, "Parsing...");
      lw_progress_set_completed (progress, FALSE);
      lw_progress_set_total (progress, content_length);
      lw_progress_set_current (progress, 0);
    }

    {
      gchar *c = contents;
      gchar *e = contents + content_length;
      gint i = 0;
      LwParsedLine *line = NULL;
      while (c < e)
      {
        while (c < e && *c == '\0') c = g_utf8_next_char (c);
        if (c >= e) break;

        line = lines + i;
        lw_parsedline_init (line);
        lw_unknowndictionary_tokenize_line (self, c, tokens, &num_tokens);
        lw_unknowndictionary_load_line_tokens (self, contents, tokens, num_tokens, line);
        if (progress != NULL)
        {
          lw_progress_set_current (progress, c - contents);
        }
        i++;
        while (c < e && *c != '\0') c = g_utf8_next_char (c);
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, content_length);
      lw_progress_set_completed (progress, TRUE);
    }

errored:

    g_free (tokens); tokens = NULL;
    if (parsed != NULL) g_object_unref (parsed); parsed = NULL;
}

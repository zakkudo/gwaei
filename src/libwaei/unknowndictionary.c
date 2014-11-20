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


G_DEFINE_TYPE (LwUnknownDictionary, lw_unknowndictionary, LW_TYPE_DICTIONARY)

static LwParsed* lw_unknowndictionary_parse (LwUnknownDictionary *self, gchar *contents, gsize content_length, LwProgress *progress);

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

    {
      static gint _column_index_types[LW_EDICTIONARYCOLUMNID_DEFINITION] = {0}:
      _column_index_types[LW_UNKNOWNDICTIONARYCOLUMNID_UNKNOWN,] = LW_DICTIONARYINDEXKEY_INDEX_AND_SEARCH;
      klass->priv->column_index_types = _column_index_types;
    }

    {
      static gchar* _column_languages[LW_EDICTIONARYCOLUMNID_DEFINITION] = {0}:
      klass->priv->column_language = _column_langages;
    }
}


/**
 * lw_unknowndictionary_columnize
 * @buffer The text to columnize.  It is columnized in place and no copy is made.
 * @columns A pointer to an alloced array to place the columns.  This array
 * should have enough space to hold the columnized buffer positions which is usually
 * (strlen(@buffer) + 1) * sizeof(gchar*).  The column array is %NULL terminated.
 * @num_columns The number of columns that were created in @columns
 *
 * Columnizes a string given the standards of edict dictionaries by placing %NULL
 * characters in the buffer, and recording the positions in the @columns array.
 * This method is made to column one line at a time.
 *
 * Returns: The end of the filled column array
 */
static gchar**
lw_unknowndictionary_columnize_line (LwUnknownDictionary  *self,
                                    gchar                *buffer,
                                    gchar               **columns,
                                    gsize                *num_columns)
{
    //Sanity checks
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (columns != NULL, NULL);

    //Declarations
    gchar *c = NULL;
    gint length = 0;

    //Initializations
    c = buffer;

    columns[length++] = c;

    columns[length] = NULL;

    if (num_columns != NULL)
    {
      *num_columns = length;
    }

    return columns + length;
}


static void
lw_unknowndictionary_load_line_columns (LwUnknownDictionary  *self,
                                       gchar                *buffer,
                                       gchar               **columns,
                                       gint                  num_columns,
                                       LwDictionaryLine     *line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_EXAMPLEDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (columns != NULL);
    g_return_if_fail (num_columns > 0);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *unknown = NULL;

    //Initializations
    unknown = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);

    {
      gint i = 0;

      if (i < num_columns)
      {
        g_array_append_val (unknown, columns[i]);
      }
    }

errored:

    g_array_set_size (unknown, unknown->len);

    lw_dictionaryline_take_strv (
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
    g_return_val_if_fail (LW_IS_EXAMPLEDICTIONARY (self), NULL);
    g_return_val_if_fail (contents != NULL, NULL);

    //Declarations
    gint num_lines = 0;
    gchar **columns = NULL;
    gsize max_line_length = 0;
    gsize num_columns = 0;
    gint length = -1;
    LwParsed *parsed = NULL;
    LwDictionaryLine* lines = NULL;

    //Initializations
    if (content_length < 1) content_length = strlen(contents);
    num_lines = lw_utf8_replace_linebreaks_with_nullcharacter (contents, content_length, &max_line_length, progress);
    if (num_lines < 1) goto errored;
    if (max_line_length < 1) goto errored;
    parsed = lw_parsed_new (contents, content_length);
    if (parsed == NULL) goto errored;
    lines = g_new0 (LwDictionaryLine, num_lines);
    if (lines == NULL) goto errored;
    columns = g_new0 (gchar*, max_line_length + 1);
    if (columns == NULL) goto errored;

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
      LwDictionaryLine *line = NULL;
      while (c < e)
      {
        while (c < e && *c == '\0') c = g_utf8_next_char (c);
        if (c >= e) break;

        line = lines + i;
        lw_dictionaryline_init (line);
        lw_unknowndictionary_columnize_line (self, c, columns, &num_columns);
        lw_unknowndictionary_load_line_columns (self, contents, columns, num_columns, line);
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

    g_free (columns); columns = NULL;
    if (parsed != NULL) lw_parsed_unref (parsed); parsed = NULL;
}


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

#include <libwaei/libwaei.h>
#include <libwaei/dictionary-private.h>
#include <libwaei/gettext.h>

static LwParsed* lw_radicalsdictionary_parse (LwRadicalsDictionary *self, gchar *contents, gsize content_length, LwProgress *progress);
static gint lw_radicalsdictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_radicalsdictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_radicalsdictionary_get_column_handling (LwDictionary *self, gint column_num);

G_DEFINE_TYPE (LwRadicalsDictionary, lw_radicalsdictionary, LW_TYPE_DICTIONARY)


LwDictionary* lw_radicalsdictionary_new (const gchar        *FILENAME,
                                        LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_RADICALSDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                 NULL));

    return dictionary;
}


static void 
lw_radicalsdictionary_init (LwRadicalsDictionary *dictionary)
{
}


static void
lw_radicalsdictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_radicalsdictionary_parent_class)->constructed (object);
    }

/*
    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;
*/
}


static void 
lw_radicalsdictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_radicalsdictionary_parent_class)->finalize (object);
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
    dictionary_class->priv->parse = (LwDictionaryParseFunc) lw_radicalsdictionary_parse;
    dictionary_class->priv->get_column_handling = lw_radicalsdictionary_get_column_handling;
    dictionary_class->priv->get_total_columns = lw_radicalsdictionary_get_total_columns;
    dictionary_class->priv->get_column_language = lw_radicalsdictionary_get_column_language;
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


/**
 * lw_edictionary_columnize
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
lw_radicalsdictionary_columnize_line (LwRadicalsDictionary  *self,
                                     gchar                 *buffer,
                                     gchar                **columns,
                                     gsize                 *num_columns)
{
    //Sanity checks
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (columns != NULL, NULL);

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

      columns[length++] = c;
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
        columns[length++] = c;

        while (*c != '\0' && !g_ascii_isspace (*c) && !g_ascii_ispunct (*c)) c = g_utf8_next_char (c);
        if (*c == '\0') goto errored;

        c = lw_utf8_set_null_next_char (c);
        if (*c == '\0') goto errored;

        while (*c != '\0' && (g_ascii_isspace (*c) || g_ascii_ispunct (*c))) c = g_utf8_next_char (c); 
        if (*c == '\0') goto errored;
      }
    }

errored:

    columns[length] = NULL;

    return columns + length;
}


static void
lw_radicalsdictionary_load_line_columns (LwRadicalsDictionary  *self,
                                       gchar                  *buffer,
                                       gchar                 **columns,
                                       gint                    num_columns,
                                       LwDictionaryLine       *line)
{
    //Sanity checks
    g_return_if_fail (LW_IS_RADICALSDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (columns != NULL);
    g_return_if_fail (num_columns > 0);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *kanji = NULL;
    GArray *radicals = NULL;
    gint i = 0;

    //Initializations
    kanji = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);
    radicals = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_columns);

    if (i < num_columns)
    {
      g_array_append_val (kanji, columns[i++]);
    }

    while (i < num_columns)
    {
      g_array_append_val (radicals, columns[i++]);
    }

errored:

    g_array_set_size (kanji, 1);
    g_array_set_size (radicals, radicals->len);

    lw_dictionaryline_take_strv (
      line,
      LW_RADICALSDICTIONARYCOLUMNID_KANJI,
      (gchar**) g_array_free (kanji, FALSE)
    );

    lw_dictionaryline_take_strv (
      line,
      LW_RADICALSDICTIONARYCOLUMNID_RADICALS,
      (gchar**) g_array_free (radicals, FALSE)
    );
}


//!
//! @brief Parses a string for an unknown format string
//! @param rl The Resultline object this method works on
//!
static LwParsed*
lw_radicalsdictionary_parse (LwRadicalsDictionary *self,
                             gchar                *contents,
                             gsize                 content_length,
                             LwProgress           *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_RADICALSDICTIONARY (self), NULL);
    g_return_val_if_fail (contents != NULL, NULL);

    //Declarations
    gint num_lines = 0;
    gchar **columns = NULL;
    gsize max_line_length = 0;
    gsize num_columns = 0;
    gint length = -1;
    LwParsed *parsed = NULL;
    LwDictionaryLine *lines = NULL;

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
        lw_radicalsdictionary_columnize_line (self, c, columns, &num_columns);
        lw_radicalsdictionary_load_line_columns (self, contents, columns, num_columns, line);
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


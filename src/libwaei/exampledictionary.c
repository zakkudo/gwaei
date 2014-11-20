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
//!  @file exampledictionary.c
//!
//!  @brief LwExampleDictionary objects represent a loaded dictionary that the program
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

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>
#include <libwaei/dictionary-private.h>

G_DEFINE_TYPE (LwExampleDictionary, lw_exampledictionary, LW_TYPE_DICTIONARY)

static LwParsed* lw_exampledictionary_parse (LwExampleDictionary *self, gchar *contents, gsize content_length, LwProgress *progress);

LwDictionary* lw_exampledictionary_new (const gchar        *FILENAME,
                                        LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_EXAMPLEDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                NULL));

    return dictionary;
}


static void 
lw_exampledictionary_init (LwExampleDictionary *dictionary)
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
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (strcmp(priv->data.filename, "Examples") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Examples"));
    }
}


static void 
lw_exampledictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_exampledictionary_parent_class)->finalize (object);
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
    dictionary_class->priv->parse = (LwDictionaryParseFunc) lw_exampledictionary_parse;
}


/**
 * lw_exampledictionary_columnize
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
lw_exampledictionary_columnize_line (LwExampleDictionary  *self,
                                    gchar                *buffer,
                                    gchar               **columns,
                                    gsize                *num_columns)
{
    //Sanity checks
    g_return_if_fail (LW_IS_EXAMPLEDICTIONARY (self));
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (columns != NULL, NULL);

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
    
    while (*c != '\0' && !g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //   ^
    //   HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}

    columns[length++] = c;

    while (*c != '\0' && !g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //                    ^
    //                    HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}

    columns[length++] = c;

    while (*c != '\0' && *c != '#') c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //                                        ^
    //                                        HERE
    //B: 直ぐに{すぐに} 戻る{戻ります}

    columns[length++] = c;

errored:

    columns[length] = NULL;

    if (num_columns != NULL)
    {
      *num_columns = length;
    }

    return columns + length;
}


static void
lw_exampledictionary_load_line_columns (LwExampleDictionary  *self,
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
    GArray *phrase = NULL;
    GArray *meaning = NULL;
    GArray *id = NULL;


    //Initializations
    phrase = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);
    meaning = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);
    id = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), 1);

    {
      gint i = 0;

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //   ^
      //   HERE
      if (columns[i] != NULL && i < num_columns)
      {
        g_array_append_val (phrase, columns[i]);
        i++;
      }

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //                    ^
      //                    HERE
      if (columns[i] != NULL && i < num_columns)
      {
        g_array_append_val (meaning, columns[i]);
        i++;
      }

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //                                         ^
      //                                         HERE
      if (columns[i] != NULL && i < num_columns)
      {
        g_array_append_val (id, columns[i]);
        i++;
      }
    }

errored:

    g_array_set_size (phrase, phrase->len);
    g_array_set_size (meaning, meaning->len);
    g_array_set_size (id, id->len);

    lw_dictionaryline_take_strv (
      line,
      LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE,
      (gchar**) g_array_free (phrase, FALSE)
    );

    lw_dictionaryline_take_strv (
      line,
      LW_EXAMPLEDICTIONARYCOLUMNID_MEANING,
      (gchar**) g_array_free (meaning, FALSE)
    );

    lw_dictionaryline_take_strv (
      line,
      LW_EXAMPLEDICTIONARYCOLUMNID_ID,
      (gchar**) g_array_free (id, FALSE)
    );
}



//!
//! @brief, Retrieve a line from FILE, parse it according to the LwExampleDictionary rules and put the results into the LwResult
//!
static LwParsed*
lw_exampledictionary_parse (LwExampleDictionary *self,
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
        lw_exampledictionary_columnize_line (self, c, columns, &num_columns);
        lw_exampledictionary_load_line_columns (self, contents, columns, num_columns, line);
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




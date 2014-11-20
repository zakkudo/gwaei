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
//!  @file edictionary.c
//!
//!  @brief LwEDictionary objects represent a loaded dictionary that the program
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

G_DEFINE_TYPE (LwEDictionary, lw_edictionary, LW_TYPE_DICTIONARY)

static LwParsed* lw_edictionary_parse (LwEDictionary *self, gchar *contents, gsize content_length, LwProgress *progress);


LwDictionary* lw_edictionary_new (const gchar        *FILENAME, 
                                  LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_EDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                NULL));

    return dictionary;
}


static void 
lw_edictionary_init (LwEDictionary *dictionary)
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
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (strcmp(priv->data.filename, "English") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("English"));
    }
    else if (strcmp(priv->data.filename, "Names") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Names"));
    }
    else if (strcmp(priv->data.filename, "Places") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Places"));
    }
    else if (strcmp(priv->data.filename, "Names and Places") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Names and Places"));
    }
}


static void 
lw_edictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_edictionary_parent_class)->finalize (object);
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
    dictionary_class->priv->parse = (LwDictionaryParseFunc) lw_edictionary_parse;

    {
      static gint _column_index_types[LW_EDICTIONARYCOLUMNID_DEFINITION] = {0}:
      _column_index_types[LW_EDICTIONARYCOLUMNID_WORD] = LW_DICTIONARYINDEXKEY_INDEX_AND_SEARCH;
      _column_index_types[LW_EDICTIONARYCOLUMNID_READING] = LW_DICTIONARYINDEXKEY_INDEX_AND_SEARCH;
      _column_index_types[LW_EDICTIONARYCOLUMNID_DEFINITION] = LW_DICTIONARYINDEXKEY_INDEX_AND_SEARCH;
      _column_index_types[LW_EDICTIONARYCOLUMNID_CLASSIFICATION] = LW_DICTIONARYINDEXKEY_FILTER_ONLY;
      _column_index_types[LW_EDICTIONARYCOLUMNID_POPULAR] = LW_DICTIONARYINDEXKEY_FILTER_ONLY;
      klass->_column_index_types = _column_index_types;
    }

    {
      static gchar* _column_languages[LW_EDICTIONARYCOLUMNID_DEFINITION] = {0}:
      _column_languages[LW_EDICTIONARYCOLUMNID_WORD] = "ja"
      _column_languages[LW_EDICTIONARYCOLUMNID_READING] = "ja"
      _column_languages[LW_EDICTIONARYCOLUMNID_DEFINITION] = "en"
      klass->_column_language = _column_langages;
    }
}


GType
lw_edictionary_columnid_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_EDICTIONARYCOLUMNID_WORD, LW_EDICTIONARYCOLUMNNAME_WORD, LW_EDICTIONARYCOLUMNNICK_WORD },
        { LW_EDICTIONARYCOLUMNID_READING, LW_EDICTIONARYCOLUMNNAME_READING, LW_EDICTIONARYCOLUMNNICK_READING },
        { LW_EDICTIONARYCOLUMNID_DEFINITION, LW_EDICTIONARYCOLUMNNAME_DEFINITION, LW_EDICTIONARYCOLUMNNICK_DEFINITION },
        { LW_EDICTIONARYCOLUMNID_CLASSIFICATION, LW_EDICTIONARYCOLUMNNAME_CLASSIFICATION, LW_EDICTIONARYCOLUMNNICK_CLASSIFICATION },
        { LW_EDICTIONARYCOLUMNID_POPULAR, LW_EDICTIONARYCOLUMNNAME_POPULAR, LW_EDICTIONARYCOLUMNNICK_POPULAR },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwEdictionaryColumnId", values);
    }

    return type;
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
lw_edictionary_columnize_line (LwEDictionary  *self,
                              gchar          *buffer,
                              gchar         **columns,
                              gsize          *num_columns)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EDICTIONARY (self), NULL);
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (columns != NULL, NULL);

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

    columns[length++] = c;

    while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //     ^
    //     HERE

    if (*c == '[')
    {
      gchar* e = c;
      while (*e !='\0' && *e !=']' && *e != '/') e = g_utf8_next_char (e);
      if (*e == '\0') goto errored;

      if (*e == ']' && e - c > 1)
      {
        columns[length++] = c;
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

      if (e - c > 1) columns[length++] = c;

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

        if (e - c > 1) columns[length++] = c;

        c = lw_utf8_set_null_next_char (e);
      }
    }

errored:

    columns[length] = NULL;

    if (num_columns != NULL)
    {
      *num_columns = length;
    }

    return columns + length;
}


static void
lw_edictionary_load_line_columns (LwEDictionary     *self,
                                 gchar             *buffer,
                                 gchar            **columns,
                                 gint               num_columns,
                                 LwDictionaryLine  *line) 
{
    //Sanity checks
    g_return_if_fail (LW_IS_EDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (columns != NULL);
    g_return_if_fail (num_columns < 1);
    g_return_if_fail (line != NULL);

    //Declarations
    GArray *word = NULL;
    GArray *reading = NULL;
    GArray *definition = NULL;
    GArray *classification = NULL;
    GArray *popular = NULL;

    //Initializations
    word = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_columns);
    reading = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_columns);
    definition = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_columns);
    classification = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_columns);
    popular = g_array_sized_new (TRUE, TRUE, sizeof(gchar*), num_columns);

    { //Get the element at the end first so the forward iteration is simpler...
      gint i = num_columns - 1;

      if (columns[i] != NULL && strcmp (columns[i], "(P)") == 0)
      {
        g_array_append_val (word, columns[i]);
        i--;
      }

      num_columns = i + 1;

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //                                                                                             ^
      //                                                                                             HERE
    }

    {
      gint i = 0;

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //^
      //HERE

      if (columns[i] != NULL && i < num_columns)
      {
        g_array_append_val (word, columns[i]);
        i++;
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //      ^
      //      HERE

      {
        GUnicodeScript script = lw_utf8_get_script (columns[i]);
        if (columns[i] != NULL && i < num_columns && script == G_UNICODE_SCRIPT_HIRAGANA || script == G_UNICODE_SCRIPT_KATAKANA)
        {
          g_array_append_val (reading, columns[i]);
          i++;
        }
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //                 ^
      //                 HERE

      {
        if (columns[i] != NULL && i < num_columns)
        {
          gchar *c = columns[i];
          while (*c != '\0')
          {
            g_array_append_val (classification, columns[i]);
            while (*c != '\0' && *c != ',') c = g_utf8_next_char (c);
            if (*c == ',') c = lw_utf8_set_null_next_char (c);
          }
          i++;
        }
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //                         ^
      //                         HERE

      while (columns[i] != NULL & i < num_columns)
      {
        g_array_append_val (definition, columns[i]);
        i++;
      }
    }

errored:

    g_array_set_size (popular, popular->len);
    g_array_set_size (word, word->len);
    g_array_set_size (reading, reading->len);
    g_array_set_size (classification, classification->len);
    g_array_set_size (definition, definition->len);

    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_POPULAR,
      (gchar**) g_array_free (popular, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_WORD,
      (gchar**) g_array_free (word, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_READING,
      (gchar**) g_array_free (reading, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_CLASSIFICATION,
      (gchar**) g_array_free (classification, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_DEFINITION,
      (gchar**) g_array_free (definition, FALSE)
    );
}


static LwParsed*
lw_edictionary_parse (LwEDictionary *self,
                      gchar         *contents,
                      gsize          content_length,
                      LwProgress    *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_EDICTIONARY (self), NULL);
    g_return_val_if_fail (contents != NULL, NULL);

    //Declarations
    gint num_lines = 0;
    LwParsed *parsed = NULL; 
    LwDictionaryLine* lines = NULL;
    gchar **columns = NULL;
    gsize max_line_length = 0;
    gsize num_columns = 0;

    //Initializations
    if (content_length < 1) content_length = strlen(contents);
    num_lines = lw_utf8_replace_linebreaks_with_nullcharacter (contents, content_length, &max_line_length, progress);
    if (num_lines == 0) goto errored;
    if (max_line_length < 1) goto errored;
    parsed = lw_parsed_new (contents, content_length);
    if (parsed == NULL) goto errored;
    lines = g_new0 (LwDictionaryLine, num_lines);
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
        lw_edictionary_columnize_line (self, c, columns, &num_columns);
        lw_edictionary_load_line_columns (self, contents, columns, num_columns, line);
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

    return parsed;
}


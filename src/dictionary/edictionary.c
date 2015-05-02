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
#include <glib-object.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>
#include <libwaei/dictionary-private.h>

#include <libwaei/dictionary/edictionary.h>

G_DEFINE_DYNAMIC_TYPE (LwEDictionary, lw_edictionary, LW_TYPE_DICTIONARY)

static gint lw_edictionary_get_total_columns (LwDictionary *self);
static gchar const * lw_edictionary_get_column_language (LwDictionary *self, gint column_num);
static LwDictionaryColumnHandling lw_edictionary_get_column_handling (LwDictionary *self, gint column_num);
static gchar** lw_edictionary_columnize (LwDictionary *self, gchar *buffer, gchar **tokens, gsize *num_tokens);
static void lw_edictionary_load_columns (LwDictionary *self, gchar *buffer, gchar **tokens, gint num_tokens, LwParsedLine *line);


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
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (strcmp(priv->filename, "English") == 0)
    {
      if (priv->name != NULL) g_free (priv->name); priv->name = NULL;
      priv->name = g_strdup (gettext("English"));
    }
    else if (strcmp(priv->filename, "Names") == 0)
    {
      if (priv->name != NULL) g_free (priv->name); priv->name = NULL;
      priv->name = g_strdup (gettext("Names"));
    }
    else if (strcmp(priv->filename, "Places") == 0)
    {
      if (priv->name != NULL) g_free (priv->name); priv->name = NULL;
      priv->name = g_strdup (gettext("Places"));
    }
    else if (strcmp(priv->filename, "Names and Places") == 0)
    {
      if (priv->name != NULL) g_free (priv->name); priv->name = NULL;
      priv->name = g_strdup (gettext("Names and Places"));
    }
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
    dictionary_class->priv->get_column_handling = lw_edictionary_get_column_handling;
    dictionary_class->priv->get_total_columns = lw_edictionary_get_total_columns;
    dictionary_class->priv->get_column_language = lw_edictionary_get_column_language;
    dictionary_class->priv->columnize = lw_edictionary_columnize;
    dictionary_class->priv->load_columns = lw_edictionary_load_columns;
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
static gchar**
lw_edictionary_columnize (LwDictionary  *self,
                          gchar         *buffer,
                          gchar        **tokens,
                          gsize         *num_tokens)
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

    return tokens + length;
}


static void
lw_edictionary_load_columns (LwDictionary  *self,
                             gchar         *buffer,
                             gchar        **tokens,
                             gint           num_tokens,
                             LwParsedLine  *line) 
{
    //Sanity checks
    g_return_if_fail (LW_IS_EDICTIONARY (self));
    g_return_if_fail (buffer != NULL);
    g_return_if_fail (tokens != NULL);
    g_return_if_fail (num_tokens < 1);
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
        g_array_append_val (word, tokens[i]);
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
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
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
            g_array_append_val (classification, tokens[i]);
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

    g_array_set_size (popular, popular->len);
    g_array_set_size (word, word->len);
    g_array_set_size (reading, reading->len);
    g_array_set_size (classification, classification->len);
    g_array_set_size (definition, definition->len);

    lw_parsedline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_POPULAR,
      (gchar**) g_array_free (popular, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_WORD,
      (gchar**) g_array_free (word, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_READING,
      (gchar**) g_array_free (reading, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_CLASSIFICATION,
      (gchar**) g_array_free (classification, FALSE)
    );
    lw_parsedline_take_strv (
      line,
      LW_EDICTIONARYCOLUMNID_DEFINITION,
      (gchar**) g_array_free (definition, FALSE)
    );
}

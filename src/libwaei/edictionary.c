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

static LwParsedDictionary* lw_edictionary_parse (LwEDictionary *self, gchar *contents, gsize content_length, LwProgress *progress);

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
}


GType
lw_edictionary_tokenid_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_EDICTIONARYTOKENID_WORD, LW_EDICTIONARYTOKENNAME_WORD, LW_EDICTIONARYTOKENNICK_WORD },
        { LW_EDICTIONARYTOKENID_READING, LW_EDICTIONARYTOKENNAME_READING, LW_EDICTIONARYTOKENNICK_READING },
        { LW_EDICTIONARYTOKENID_DEFINITION, LW_EDICTIONARYTOKENNAME_DEFINITION, LW_EDICTIONARYTOKENNICK_DEFINITION },
        { LW_EDICTIONARYTOKENID_CLASSIFICATION, LW_EDICTIONARYTOKENNAME_CLASSIFICATION, LW_EDICTIONARYTOKENNICK_CLASSIFICATION },
        { LW_EDICTIONARYTOKENID_POPULAR, LW_EDICTIONARYTOKENNAME_POPULAR, LW_EDICTIONARYTOKENNICK_POPULAR },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwEdictionaryTokenId", values);
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
 * Tokenizes a string given the standards of edict dictionaries by placing %NULL
 * characters in the buffer, and recording the positions in the @tokens array.
 * This method is made to token one line at a time.
 *
 * Returns: The end of the filled token array
 */
static gchar**
lw_edictionary_tokenize_line (LwEDictionary  *self,
                              gchar          *buffer,
                              gchar         **tokens,
                              gsize          *num_tokens)
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
lw_edictionary_load_line_tokens (LwEDictionary     *self,
                                 gchar             *buffer,
                                 gchar            **tokens,
                                 gint               num_tokens,
                                 LwDictionaryLine  *line) 
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

    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYTOKENID_POPULAR,
      (gchar**) g_array_free (popular, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYTOKENID_WORD,
      (gchar**) g_array_free (word, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYTOKENID_READING,
      (gchar**) g_array_free (reading, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYTOKENID_CLASSIFICATION,
      (gchar**) g_array_free (classification, FALSE)
    );
    lw_dictionaryline_take_strv (
      line,
      LW_EDICTIONARYTOKENID_DEFINITION,
      (gchar**) g_array_free (definition, FALSE)
    );
}


static LwParsedDictionary*
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
    LwParsedDictionary *lines = NULL; 
    gchar **tokens = NULL;
    gsize max_line_length = 0;
    gsize num_tokens = 0;

    //Initializations
    if (content_length < 1) content_length = strlen(contents);
    num_lines = lw_utf8_replace_linebreaks_with_nullcharacter (contents, content_length, &max_line_length, progress);
    if (num_lines == 0) goto errored;
    if (max_line_length < 1) goto errored;
    lines = lw_parseddictionary_new (num_lines);
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
      LwDictionaryLine *line = NULL;
      while (c < e)
      {
        while (c < e && *c == '\0') c = g_utf8_next_char (c);
        if (c >= e) break;

        line = lw_parseddictionary_get_line (lines, i);
        lw_edictionary_tokenize_line (self, c, tokens, &num_tokens);
        lw_edictionary_load_line_tokens (self, contents, tokens, num_tokens, line);
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
    if (lines != NULL) lw_parseddictionary_unref (lines); lines = NULL;

    return lines;
}


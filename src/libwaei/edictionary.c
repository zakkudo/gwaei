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

static void lw_edictionary_load_tokens (LwDictionary *self, LwDictionaryLine *line, gchar **token_buffer, gint num_tokens);
static gchar** lw_edictionary_tokenize (LwDictionary *self, gchar *buffer, gchar **tokens, gint *num_tokens);

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

    if (strcmp(priv->config.filename, "English") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("English"));
    }
    else if (strcmp(priv->config.filename, "Names") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Names"));
    }
    else if (strcmp(priv->config.filename, "Places") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Places"));
    }
    else if (strcmp(priv->config.filename, "Names and Places") == 0)
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
    dictionary_class->priv->tokenize = lw_edictionary_tokenize;
    dictionary_class->priv->load_tokens = lw_edictionary_load_tokens;
}


GType
lw_edictionary_tokenid_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_EDICTIONARYTOKENID_WORD, LW_EDICTIONARYTOKENNAME_WORD, "word" },
        { LW_EDICTIONARYTOKENID_READING, LW_EDICTIONARYTOKENNAME_READING, "reading" },
        { LW_EDICTIONARYTOKENID_DEFINITION, LW_EDICTIONARYTOKENNAME_DEFINITION, "definition" },
        { LW_EDICTIONARYTOKENID_CLASSIFICATION, LW_EDICTIONARYTOKENNAME_CLASSIFICATION, "classification" },
        { LW_EDICTIONARYTOKENID_POPULAR, LW_EDICTIONARYTOKENNAME_POPULAR, "popular" },
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
lw_edictionary_tokenize (LwDictionary   *self,
                         gchar          *buffer,
                         gchar         **tokens,
                         gint           *num_tokens)
{
    //Sanity checks
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


//!
//! @brief, Retrieve a line from FILE, parse it according to the LwEDictionary rules and put the results into the LwResult
//!
static void
lw_edictionary_load_tokens (LwDictionary      *self,
                            LwDictionaryLine  *line,
                            gchar            **token_buffer,
                            gint               num_tokens)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (line != NULL, NULL);
    g_return_val_if_fail (token_buffer != NULL, NULL);

    //Declarations
    /*TODO
    LwResultBuffer words = {0};
    LwResultBuffer readings = {0};
    LwResultBuffer definitions = {0};
    LwResultBuffer classifications = {0};
    LwResultBuffer popular = {0};

    //Initializations
    result = lw_result_new (TEXT);
    if (result == NULL) goto errored;
    tokens = _tokenize (result, &length);

    lw_result_init_buffer (result, &words);
    lw_result_init_buffer (result, &readings);
    lw_result_init_buffer (result, &definitions);
    lw_result_init_buffer (result, &classifications);
    lw_result_init_buffer (result, &popular);

    { //Get the element at the end first so the forward iteration is simpler...
      gint i = length - 1;

      if (tokens[i] != NULL && strcmp (tokens[i], "(P)") == 0)
      {
        lw_resultbuffer_add (&popular, gettext("popular"));
        i--;
      }

      length = i + 1;

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //                                                                                             ^
      //                                                                                             HERE
    }

    {
      gint i = 0;

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //^
      //HERE

      if (tokens[i] != NULL && i < length)
      {
        lw_resultbuffer_add (&words, tokens[i]);
        i++;
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //      ^
      //      HERE

      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (tokens[i] != NULL && i < length && script == G_UNICODE_SCRIPT_HIRAGANA || script == G_UNICODE_SCRIPT_KATAKANA)
        {
          lw_resultbuffer_add (&readings, tokens[i]);
          i++;
        }
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //                 ^
      //                 HERE

      {
        if (tokens[i] != NULL && i < length)
        {
          gchar *c = tokens[i];
          while (*c != '\0')
          {
            lw_resultbuffer_add (&classifications, tokens[i]);
            while (*c != '\0' && *c != ',') c = g_utf8_next_char (c);
            if (*c == ',') c = lw_utf8_set_null_next_char (c);
          }
          i++;
        }
      }

      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month[INVISIBLE SECTION]
      //                         ^
      //                         HERE

      while (tokens[i] != NULL & i < length)
      {
        lw_resultbuffer_add (&definitions, tokens[i]);
        i++;
      }
    }

    lw_result_take_buffer (result, LW_EDICTIONARY_KEY_WORD, &words);
    lw_result_take_buffer (result, LW_EDICTIONARY_KEY_READING, &readings);
    lw_result_take_buffer (result, LW_EDICTIONARY_KEY_DEFINITION, &definitions);
    lw_result_take_buffer (result, LW_EDICTIONARY_KEY_CLASSIFICATION, &classifications);
    lw_result_take_buffer (result, LW_EDICTIONARY_KEY_POPULAR, &popular);

errored:

    lw_resultbuffer_clear (&words, TRUE);
    lw_resultbuffer_clear (&readings, TRUE);
    lw_resultbuffer_clear (&definitions, TRUE);
    lw_resultbuffer_clear (&classifications, TRUE);
    lw_resultbuffer_clear (&popular, TRUE);
*/
}


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

static LwResult* lw_exampledictionary_parse (LwDictionary*, const gchar*);
gchar** lw_exampledictionary_tokenize (gchar  *buffer, gchar **tokens, gint *num_tokens);


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

    if (strcmp(priv->config.filename, "Examples") == 0)
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
    dictionary_class->priv->parse = lw_exampledictionary_parse;
    dictionary_class->priv->tokenize = lw_exampledictionary_tokenize;
}


/**
 * lw_exampledictionary_tokenize
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
gchar**
lw_exampledictionary_tokenize (gchar  *buffer,
                               gchar **tokens,
                               gint   *num_tokens)
{
    //Sanity checks
    g_return_val_if_fail (buffer != NULL, NULL);
    g_return_val_if_fail (tokens != NULL, NULL);

    //Declarations
    gchar *c = NULL;
    gint length = 0;

    //Initializations
    c = buffer;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //B: 直ぐに{すぐに} 戻る{戻ります}
    //^
    //HERE
    
    while (*c != '\0' && !g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //B: 直ぐに{すぐに} 戻る{戻ります}
    //   ^
    //   HERE

    tokens[length++] = c;

    while (*c != '\0' && !g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //B: 直ぐに{すぐに} 戻る{戻ります}
    //                    ^
    //                    HERE

    tokens[length++] = c;

    while (*c != '\0' && *c != '#') c = g_utf8_next_char (c);
    if (*c == '\0') goto errored;

    c = lw_utf8_set_null_next_char (c);
    if (*c == '\0') goto errored;

    //An example line
    //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
    //B: 直ぐに{すぐに} 戻る{戻ります}
    //                                        ^
    //                                        HERE

    tokens[length++] = c;

errored:

    tokens[length] = NULL;

    if (num_tokens != NULL)
    {
      *num_tokens = length;
    }

    return tokens + length;
}


//!
//! @brief, Retrieve a line from FILE, parse it according to the LwExampleDictionary rules and put the results into the LwResult
//!
static LwResult*
lw_exampledictionary_parse (LwDictionary *dictionary, 
                            const gchar  *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);
    if (TEXT == NULL) return NULL;

    //Declarations
    LwResult *result = NULL;
    gchar **tokens = NULL;
    gint length = -1;
    LwResultBuffer phrase = {0};
    LwResultBuffer meaning = {0};
    LwResultBuffer id = {0};

    /*TODO

    //Initializations
    result = lw_result_new (TEXT);
    if (result == NULL) goto errored;
    tokens = _tokenize (result, &length);
    if (tokens == NULL) goto errored;

    lw_result_init_buffer (result, &phrase);
    lw_result_init_buffer (result, &meaning);
    lw_result_init_buffer (result, &id);

    {
      gint i = 0;

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //   ^
      //   HERE
      if (tokens[i] != NULL && i < length)
      {
        lw_resultbuffer_add (&phrase, tokens[i]);
        i++;
      }

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //                    ^
      //                    HERE
      if (tokens[i] != NULL && i < length)
      {
        lw_resultbuffer_add (&meaning, tokens[i]);
        i++;
      }

      //An example line
      //A: すぐに戻ります。 I will be back soon.#ID=1284_4709
      //B: 直ぐに{すぐに} 戻る{戻ります}
      //                                         ^
      //                                         HERE
      if (tokens[i] != NULL && i < length)
      {
        lw_resultbuffer_add (&id, tokens[i]);
        i++;
      }
    }

    lw_result_take_buffer (result, LW_EXAMPLEDICTIONARY_KEY_PHRASE, &phrase);
    lw_result_take_buffer (result, LW_EXAMPLEDICTIONARY_KEY_MEANING, &meaning);
    lw_result_take_buffer (result, LW_EXAMPLEDICTIONARY_KEY_ID, &id);

errored:

    lw_resultbuffer_clear (&phrase, TRUE);
    lw_resultbuffer_clear (&meaning, TRUE);
    lw_resultbuffer_clear (&id, TRUE);

    return result;

/*TODO
    //Declarations
    gchar *ptr;
    //gint length;

    //Initializations
    result->text = ptr = g_strdup (TEXT);

    if (!lw_exampledictionary_is_a (result->text)) goto errored;

    //Set the kanji string
    ptr = result->kanji_start = result->text + 3;

    //Set the romaji string
    while (*ptr != '\0' && !g_unichar_isspace (g_utf8_get_char (ptr))) ptr = g_utf8_next_char (ptr);
    if (*ptr == '\0') goto errored;
    *ptr = '\0';
    ptr++;
    result->def_start[0] = ptr;

    //Erase the id number
    while (*ptr != '\0' && *ptr != '#') ptr = g_utf8_next_char (ptr);
    *(ptr++) = '\0';

    while (*ptr != '\n') ptr++;
    *ptr = '\0';

    //Set the "furigana" string
    ptr = fgets(ptr, LW_IO_MAX_FGETS_LINE - length, fd);
    if (ptr != NULL && lw_exampledictionary_is_b (ptr))
    {
      result->furigana_start = ptr + 3;
      
      length = strlen(ptr);
      ptr += length - 1;

      if (*ptr == '\n') *ptr = '\0';
    }

errored:

    return 1;
*/
}




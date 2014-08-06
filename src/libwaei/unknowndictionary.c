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

static LwResult* lw_unknowndictionary_parse (LwDictionary*, const gchar*);
static gchar** lw_unknowndictionary_tokenize (gchar *buffer, gchar **tokens, gint *num_tokens);


G_DEFINE_TYPE (LwUnknownDictionary, lw_unknowndictionary, LW_TYPE_DICTIONARY)


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
    dictionary_class->priv->parse = lw_unknowndictionary_parse;
    dictionary_class->priv->tokenize = lw_unknowndictionary_tokenize;
}


/**
 * lw_unknowndictionary_tokenize
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
lw_unknowndictionary_tokenize (gchar   *buffer,
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

    tokens[length++] = c;

    tokens[length] = NULL;

    if (num_tokens != NULL)
    {
      *num_tokens = length;
    }

    return tokens + length;
}


//!
//! @brief Parses a string for an unknown format string
//! @param rl The Resultline object this method works on
//!
static LwResult*
lw_unknowndictionary_parse (LwDictionary       *dictionary, 
                            const gchar        *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);
    if (TEXT == NULL) return NULL;

    //Declarations
    LwResult *result = NULL;
    gchar *c = NULL;
    LwResultBuffer text = {0};

    //Initializations
    result = lw_result_new (TEXT);
    if (result == NULL) goto errored;
    c = lw_result_get_innerbuffer (result);
    if (c == NULL) goto errored;

    lw_result_init_buffer (result, &text);

    lw_resultbuffer_add (&text, c);

    lw_result_take_buffer (result, LW_UNKNOWNDICTIONARY_KEY_TEXT, &text);

errored:

    lw_resultbuffer_clear (&text, TRUE);

    return result;
}


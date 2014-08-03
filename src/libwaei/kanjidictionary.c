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
//!  @file kanjidictionary.c
//!
//!  @brief LwKanjiDictionary objects represent a loaded dictionary that the program
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

G_DEFINE_TYPE (LwKanjiDictionary, lw_kanjidictionary, LW_TYPE_DICTIONARY)

static LwResult* lw_kanjidictionary_parse (LwDictionary*, const gchar*);


LwDictionary* lw_kanjidictionary_new (const gchar        *FILENAME, 
                                      LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_KANJIDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                NULL));

    return dictionary;
}


static void 
lw_kanjidictionary_init (LwKanjiDictionary *dictionary)
{
}


static void
lw_kanjidictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_kanjidictionary_parent_class)->constructed (object);
    }

    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (strcmp(priv->config.filename, "Kanji") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Kanji"));
    }

}


static void 
lw_kanjidictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_kanjidictionary_parent_class)->finalize (object);
}


static void
lw_kanjidictionary_class_init (LwKanjiDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_kanjidictionary_finalize;
    object_class->constructed = lw_kanjidictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->priv->parse = lw_kanjidictionary_parse;
}


gboolean
_is_stroke_number (const gchar *c)
{
  return (*c == 'S' && g_ascii_isdigit(*(c + 1)));
}


static gchar**
_tokenize (LwResult *result, gint *length)
{
    //Sanity checks
    g_return_val_if_fail (result != NULL, NULL);

    //逢 3029 U9022 B162 G9 S10 F2116 N4694 V6054 DP4002 DL2774 L2417 DN2497 O1516 MN38901X MP11.0075 P3-3-7 I2q7.15 Q3730.4 DR2555 ZRP3-4-7 Yfeng2 Wbong ホウ あ.う T1 おう {meeting}  

    //Declarations
    LwResultBuffer buffer = {0};
    gchar *c = NULL;
    gchar *e = NULL;

    //Initializations
    lw_result_init_buffer (result, &buffer);
    c = lw_result_get_innerbuffer (result);
    if (c == NULL) goto errored;

    while (*c != '\0')
    {
      while (*c != '\0' && g_ascii_isspace (*c)) c = g_utf8_next_char (c); //Find the next token start
      if (*c == '\0') break;

      if (*c == '{') //Tokens can be multiple words long between {}
      {
        while (*c != '\0' && *c == '{') c = g_utf8_next_char (c);
        if (*c == '\0') break;

        e = c;
        {
          while (*e != '\0' && *e != '}') e = g_utf8_next_char (e); //Make sure the end of the token exists }
          if (*e == '\0') break;
    
          if (e - c > 1) lw_resultbuffer_add (&buffer, c);
        }
        c = lw_utf8_set_null_next_char (e);
        if (*c == '\0') break;
      }
      else //Every space denotes a token
      {
        e = c;
        {
          while (*e != '\0' && !g_ascii_isspace (*e)) e = g_utf8_next_char (e);

          if (e - c > 1) lw_resultbuffer_add (&buffer, c);
        }
        c = lw_utf8_set_null_next_char (e);
        if (*c == '\0') break;
      }
    }

    if (length != NULL)
    {
      *length = lw_resultbuffer_length (&buffer);
    }

errored:

    return lw_resultbuffer_clear (&buffer, FALSE);
}



//!
//! @brief, Retrieve a line from FILE, parse it according to the LwKanjiDictionary rules and put the results into the LwResult
//!
static LwResult*
lw_kanjidictionary_parse (LwDictionary       *dictionary, 
                          const gchar        *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);
    if (TEXT == NULL) return NULL;

    //Declarations
    LwResult *result = NULL;
    gchar **tokens = NULL;
    gint length = -1;
    LwResultBuffer kanji = {0};
    LwResultBuffer unicode_symbol = {0};
    LwResultBuffer usage_frequency = {0};
    LwResultBuffer stroke_count = {0};
    LwResultBuffer grade_level = {0};
    LwResultBuffer jlpt_level = {0};
    LwResultBuffer kun_readings = {0};
    LwResultBuffer on_readings = {0};
    LwResultBuffer meanings = {0};

    //Initializations
    result = lw_result_new (TEXT);
    if (result == NULL) goto errored;
    tokens = _tokenize (result,  &length);
    if (tokens == NULL) goto errored;

    lw_result_init_buffer (result, &kanji);
    lw_result_init_buffer (result, &unicode_symbol);
    lw_result_init_buffer (result, &usage_frequency);
    lw_result_init_buffer (result, &stroke_count);
    lw_result_init_buffer (result, &grade_level);
    lw_result_init_buffer (result, &jlpt_level);
    lw_result_init_buffer (result, &kun_readings);
    lw_result_init_buffer (result, &on_readings);
    lw_result_init_buffer (result, &meanings);

    { //Calculate the meanings now to make things easier on the forward iteration...
      gint i = length - 1;

      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう {meeting}  
      //                                                           ^
      //                                                           HERE

      do
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script == G_UNICODE_SCRIPT_KATAKANA || script == G_UNICODE_SCRIPT_HIRAGANA) break;

        if (tokens[i] != NULL) lw_resultbuffer_add (&meanings, tokens[i]);
        i--;
      } while (i > 0);
      
      length = i + 1;
    }

    {
      gint i = 0;


      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう [THIS SECTION IS NOW INVISIBLE]
      //^
      //HERE

      if (tokens[i] != NULL && i < length) 
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script == G_UNICODE_SCRIPT_HAN)
        {
          lw_resultbuffer_add (&kanji, tokens[i]);
          i++;
        }
      }

      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう [THIS SECTION IS NOW INVISIBLE]
      //   ^                                    ^
      //   HERE                                 END

      while (tokens[i] != NULL && i < length) 
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script == G_UNICODE_SCRIPT_KATAKANA || script == G_UNICODE_SCRIPT_HIRAGANA) break;
/*TODO
        if (_is_unicode_symbol (tokens[i]))
        {
          lw_resultbuffer_add (&unicode_symbol, tokens[i]);
        }
        else if (_is_usage_frequency (tokens[i]))
        {
          lw_resultbuffer_add (&usage_frequency, tokens[i]);
        }
        else if (_is_stroke_number (tokens[i]))
        {
          lw_resultbuffer_add (&stroke_number, tokens[i]);
        }
        else if (_is_grade_number (tokens))
        {
          lw_resultbuffer_add (&grade_number, tokens[i]);
        }
        else if (_is_jlpt_level (tokens))
        {
          lw_resultbuffer_add (&jlpt_level, tokens[i]);
        }
*/
        i++;
      }

      //An example line
      //逢 3029 U9022 B162 G9 S10  Yfeng2 Wbong ホウ あ.う T1 おう [THIS SECTION IS NOW INVISIBLE]
      //                                        ^          ^
      //                                        HERE       END

      while (tokens[i] != NULL && i < length) 
      {
        GUnicodeScript script = lw_utf8_get_script (tokens[i]);
        if (script != G_UNICODE_SCRIPT_KATAKANA && script != G_UNICODE_SCRIPT_HIRAGANA) break;
/*TODO
        if (_is_kun_reading (tokens[i]))
        {
          lw_resultbuffer_add (&kun_reading, c);
        }
        else if (_is_on_reading (tokens[i]))
        {
          lw_resultbuffer_add (&on_reading, c);
        }
*/

        i++;
      }

    }

    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_KANJI, &kanji);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_UNICODE_SYMBOL, &unicode_symbol);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_USAGE_FREQUENCY, &usage_frequency);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_STROKE_COUNT, &stroke_count);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_GRADE_LEVEL, &grade_level);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_JLPT_LEVEL, &jlpt_level);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_KUN_READINGS, &kun_readings);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_ON_READINGS, &on_readings);
    lw_result_take_buffer (result, LW_KANJIDICTIONARY_KEY_MEANINGS, &meanings);

errored:

    lw_resultbuffer_clear (&kanji, TRUE);
    lw_resultbuffer_clear (&unicode_symbol, TRUE);
    lw_resultbuffer_clear (&usage_frequency, TRUE);
    lw_resultbuffer_clear (&stroke_count, TRUE);
    lw_resultbuffer_clear (&grade_level, TRUE);
    lw_resultbuffer_clear (&jlpt_level, TRUE);
    lw_resultbuffer_clear (&kun_readings, TRUE);
    lw_resultbuffer_clear (&on_readings, TRUE);
    lw_resultbuffer_clear (&meanings, TRUE);

    return result;
}





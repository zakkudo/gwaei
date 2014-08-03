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
//! @file utf8.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>

gchar*
lw_utf8_set_null_next_char (gchar *TEXT)
{
    //Declarations
    gchar *c = NULL;

    //Initializations
    c = TEXT;

    if (c != NULL && *c != '\0')
    {
      gchar *next = g_utf8_next_char (c);
      *c = '\0';
      c = next;
    }

    return c;
}


gint*
lw_utf8_get_numbers (const gchar *TEXT)
{
    if (TEXT == NULL) return NULL;

    //Declarations
    gint length = -1;
    gint *numbers = NULL;

    //Initializations
    length = strlen(TEXT);
    numbers = g_new0 (gint, length + 1);

    //Find the numbers in the filename
    {
      gint j = 0;
      const gchar *c = TEXT;
      gchar *endptr = NULL;
      gint n = -1;

      while (*c != '\0' && (c - TEXT) < length)
      {
        while (*c != '\0' && !isdigit(*c)) c++;

        n = (gint) strtol (c, &endptr, 10);

        if (c != endptr && n > 0)
        {
          numbers[j++] = n;
          c = endptr;
        }
      }
    }

    return numbers;
}


gint
lw_utf8_replace_linebreaks_with_nullcharacter (gchar *TEXT)
{
    g_return_if_fail (TEXT != NULL);

    //Declarations    
    gint length = NULL;
    gchar *c = NULL;

    //Initializations
    length = strlen(TEXT);
    c = TEXT;

    while (c < TEXT + length)
    {
      if (*c == '\n') *c = '\0';
      c++;
    }

    return length;
}



//!
//! @brief Will change a query into a & delimited set of tokens (logical and)
//!
gchar*
lw_utf8_normalize (const gchar          *TEXT,
                   LwNormalizationFlags  flags)
{
    //Sanity checks
    if (TEXT == NULL) return NULL;

    //Declarations
    gchar *buffer = NULL;
    gchar *temp = NULL;
    
    //Initializations
    buffer = g_utf8_normalize (TEXT, -1, G_NORMALIZE_ALL);
    if (buffer == NULL) goto errored;

    if (flags | LW_NORMALIZATION_CASE_INSENSITIVE)
    {
      temp = g_utf8_casefold (buffer, -1);
      g_free (buffer); buffer = temp; temp = NULL;
    }

    if (flags | LW_NORMALIZATION_FURIGANA_INSENSITIVE)
    {
      temp = lw_utf8_furiganafold (buffer); 
      g_free (buffer); buffer = temp; temp = NULL;
    }

errored:

    return buffer;
}


static GHashTable*
_lw_utf8_get_furiganafold_hashtable ()
{
    //Declarations
    GHashTable *table = NULL;
    const gchar **ptr = NULL;
    gunichar key = NULL;
    gunichar value = NULL;
    static const gchar* conversions[] = {
      "ア", "あ",
      "イ", "い",
      "ウ", "う",
      "エ", "え",
      "オ", "お",

      "ァ", "ぁ",
      "ィ", "ぃ",
      "ゥ", "ぅ",
      "ェ", "ぇ",
      "ォ", "ぉ",

      "カ", "か",
      "キ", "き",
      "ク", "く",
      "ケ", "け",
      "コ", "こ",

      "ガ", "が",
      "ギ", "ぎ",
      "グ", "ぐ",
      "ゲ", "げ",
      "ゴ", "ご",

      "マ", "ま",
      "ミ", "み",
      "ム", "む",
      "メ", "め",
      "モ", "も",

      "ナ", "な",
      "二", "に",
      "ヌ", "ぬ",
      "ネ", "ね",
      "ノ", "の",

      "サ", "さ",
      "シ", "し",
      "ス", "す",
      "セ", "せ",
      "ソ", "そ",

      "ザ", "ざ",
      "ジ", "じ",
      "ズ", "ず",
      "ゼ", "ぜ",
      "ゾ", "ぞ",

      "タ", "た",
      "チ", "ち",
      "ツ", "つ",
      "テ", "て",
      "ト", "と",

      "ダ", "だ",
      "ヂ", "ぢ",
      "ヅ", "づ",
      "デ", "で",
      "ド", "ど",

      "ハ", "は",
      "ヒ", "ひ",
      "フ", "ふ",
      "ヘ", "へ",
      "ホ", "ほ",

      "パ", "ぱ",
      "ピ", "ぴ",
      "プ", "ぷ",
      "ぺ", "ぺ",
      "ポ", "ぽ",

      "バ", "ば",
      "ビ", "び",
      "ブ", "ぶ",
      "ベ", "べ",
      "ボ", "ぼ",

      "ヤ", "や",
      "ユ", "ゆ",
      "ヨ", "よ",

      "ャ", "ゃ",
      "ュ", "ゅ",
      "ョ", "ょ",

      "っ", "つ",
      "ッ", "つ",

      NULL
    };

    table = g_hash_table_new (g_direct_hash, g_direct_equal);
    if (table == NULL) return NULL;
    ptr = conversions;

    while (*ptr != NULL && *(ptr + 1) != NULL)
    {
      key = g_utf8_get_char (*(ptr++));
      value = g_utf8_get_char (*(ptr++));
      g_hash_table_insert (table, GUINT_TO_POINTER (key), GUINT_TO_POINTER (value));
    }

    return table;    
}


gchar*
lw_utf8_furiganafold (const gchar *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);

    //Declarations
    gchar *buffer;
    gchar *ptr;
    GHashTable *table;
    gunichar c, replacement;
    
    //Initializations
    buffer = g_strdup (TEXT);
    if (buffer == NULL) goto errored;
    table = _lw_utf8_get_furiganafold_hashtable ();
    if (table == NULL) goto errored;

    for (ptr = buffer; *ptr != '\0'; ptr = g_utf8_next_char (ptr))
    {
      c = g_utf8_get_char (ptr);
      replacement = GPOINTER_TO_UINT (g_hash_table_lookup (table, GUINT_TO_POINTER (c)));
      if (replacement) g_unichar_to_utf8 (replacement, ptr);
    }

    if (!g_utf8_validate (buffer, -1, NULL)) 
    {
      g_free (buffer); buffer = NULL;
    }

errored:

    if (table != NULL) g_hash_table_unref (table); table = NULL;

    return buffer;
}


//!
//! @brief Sanitize an input string
//!
//! This function will check if the input string is a valid utf-8 sequence,
//! it will then normalize this string in the Normalization Form Canonical Composition,
//! then replace the bytes of unprintable unicode glyphe (like control codepoint) with spaces,
//! and finally will remove leading and trailing spaces if asked to.
//!
//! @param text an utf8 encoded string to sanitize
//! @param strip if true remove leading and trailing spaces
//! @return a newly allocated sanitized utf8 encoded string or NULL if text was too.
//!         If the result is non-NULL it must be freed with g_free(). 
//!
void
lw_utf8_sanitize (gchar *buffer)
{
    //Sanity Check
    g_return_if_fail (buffer != NULL);
    
    //Declarations
    const gchar *end; 
    gchar *ptr;
    gunichar c;
    gint bytes;

    //Validate the string as proper utf8
    if (!g_utf8_validate (buffer, -1, &end)) *((gchar*)end) = '\0'; 
      
    //Clear unprintable characters
    for (ptr = buffer; *ptr != '\0'; ptr = g_utf8_next_char (ptr))
    {
      c = g_utf8_get_char (ptr);
      if (!g_unichar_isprint (c))
      {
        bytes = g_unichar_to_utf8 (c, NULL);
        memset(ptr, ' ', bytes);
      }
    }

    //Strip any possible whitespace at the beginning or end
    g_strstrip (buffer); 
}


GUnicodeScript
lw_utf8_get_script (gchar *TEXT)
{
    if (TEXT == NULL) return G_UNICODE_SCRIPT_INVALID_CODE;

    //Declarations
    GUnicodeScript script = G_UNICODE_SCRIPT_INVALID_CODE;
    gunichar c = 0;
    
    //Initializations
    c =g_utf8_get_char (TEXT);
    script = g_unichar_get_script (c);

    return script;
}



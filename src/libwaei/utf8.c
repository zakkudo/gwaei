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

GQuark
lw_utf8_error_quark ()
{
    return g_quark_from_static_string ("lw-utf8-error");
}


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


GType
lw_utf8flag_get_type ()
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
  {
    static GFlagsValue const values[] = {
      { LW_UTF8FLAG_NONE, LW_UTF8FLAGNAME_NONE, LW_UTF8FLAGNICK_NONE },
      { LW_UTF8FLAG_PRINTABLE, LW_UTF8FLAGNAME_PRINTABLE, LW_UTF8FLAGNICK_PRINTABLE },
      { LW_UTF8FLAG_COMPARABLE, LW_UTF8FLAGNAME_COMPARABLE, LW_UTF8FLAGNICK_COMPARABLE },
      { LW_UTF8FLAG_CASEFOLD, LW_UTF8FLAGNAME_CASEFOLD, LW_UTF8FLAGNICK_CASEFOLD },
      { LW_UTF8FLAG_FURIGANAFOLD, LW_UTF8FLAGNAME_FURIGANAFOLD, LW_UTF8FLAGNICK_FURIGANAFOLD },
      { LW_UTF8FLAG_ALL, LW_UTF8FLAGNAME_ALL, LW_UTF8FLAGNICK_ALL },
      { 0, NULL, NULL },
    };

    type = g_flags_register_static ("LwUtf8Flag", values);
  }

  return type;
}


LwUtf8Flag
lw_utf8flag_clean (LwUtf8Flag flags)
{
    if (LW_UTF8FLAG_COMPARABLE & flags)
    {
      flags &= (~LW_UTF8FLAG_PRINTABLE);
    }

    flags &= LW_UTF8FLAG_ALL;

    return flags;
}


gboolean
lw_utf8_validate (const gchar *TEXT,
                  gint         length,
                  LwProgress  *progress)
{
    //Sanity checks
    if (TEXT == NULL) return FALSE;

    //Declarations
    gint chunk_size = 0;
    gint chunk = 0;
    gint i = 0;
    gunichar c = 0;
    const gchar *p = 0;
    gboolean is_valid = FALSE;
    gsize offset = 0;

    //Initializations
    chunk_size = lw_progress_get_chunk_size (progress);
    if (chunk_size < 1) goto errored;
    p = TEXT;
    if (p == NULL) goto errored;
    is_valid = TRUE;

    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "%s...", gettext("Validating"));
      lw_progress_set_total (progress, length);
      lw_progress_set_current (progress, 0);
      lw_progress_set_completed (progress, FALSE);
    }

    while (*p != '\0' && i < length)
    {
      offset = p - TEXT;
      c = g_utf8_get_char_validated (p, length - offset);
      if (G_UNLIKELY (c == -1))
      {
        if (progress != NULL)
        {
          lw_progress_take_error (progress, g_error_new (
            LW_UTF8_ERROR,
            LW_UTF8_ERRORCODE_INVALID_CHARACTER,
            "Invalid utf8 character at offset %d",
            offset
          ));
        }
        is_valid = FALSE;
        goto errored;
      }

      if (G_UNLIKELY (chunk >= chunk_size))
      {
        if (progress != NULL) lw_progress_set_current (progress, offset);
        chunk = 0;
      }
      else
      {
        chunk++;
      }
    }

errored:

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
      lw_progress_set_completed (progress, TRUE);
    }

    return is_valid;
}


//!
//! @brief Will change a query into a & delimited set of tokens (logical and)
//!
gchar *
lw_utf8_normalize (const gchar * TEXT,
                   gssize        length,
                   LwUtf8Flag    flags)
{
    //Sanity checks
    if (TEXT == NULL) return NULL;

    //Declarations
    gchar *buffer = NULL;
    gchar *temp = NULL;
    
    //Initializations
    if (flags & LW_UTF8FLAG_COMPARABLE)
    {
      buffer = g_utf8_normalize (TEXT, -1, G_NORMALIZE_ALL);
    }
    else if (flags & LW_UTF8FLAG_PRINTABLE)
    {
      buffer = g_utf8_normalize (TEXT, -1, G_NORMALIZE_DEFAULT);
    }
    else
    {
      buffer = g_strdup (TEXT);
    }

    if (flags & LW_UTF8FLAG_CASEFOLD)
    {
      lw_utf8_casefold (buffer, length, NULL);
    }

    if (flags & LW_UTF8FLAG_FURIGANAFOLD)
    {
      lw_utf8_furiganafold (buffer, length, NULL);
    }

errored:

    return buffer;
}


gint
lw_utf8_normalize_chunk (gchar       **output_chunk,
                         const gchar  *TEXT,
                         LwUtf8Flag    flags,
                         gssize        max_length)
{
    //Sanity checks
    if (TEXT == NULL) return 0;

    //Declarations
    const gchar *e = TEXT;
    const gchar *p = NULL;
    gint bytes_read = 0;

    while (*e != '\0' && bytes_read <= max_length)
    {
      p = e;
      e = g_utf8_next_char (e);
      bytes_read = e - TEXT;
    }
    if (p != NULL) e = p;


    *output_chunk = lw_utf8_normalize (TEXT, bytes_read, flags);

    return e - TEXT;
}


void
lw_utf8_normalize_chunked (gchar const        * CONTENTS, 
                           gsize                content_length,
                           LwUtf8Flag           flags,
                           LwUtf8ChunkHandler   chunk_handler,
                           gpointer             chunk_handler_data,
                           LwProgress         * progress)
{
    //Sanity checks
    g_return_if_fail (CONTENTS != NULL);
    g_return_if_fail (content_length > 0);
    g_return_if_fail (chunk_handler != NULL);
    g_return_if_fail (LW_IS_PROGRESS (progress));
    if (lw_progress_should_abort (progress)) return;

		//Declarations
		gint bytes_read = -1;
		gsize handled_bytes = 0;
		gsize bytes_normalized = 0;
		const char *C = CONTENTS;
		gsize left = content_length;
		gchar *normalized = NULL;
    gsize chunk_size = 0;
    GError *error = NULL;
    gboolean has_error = FALSE;

    chunk_size = lw_progress_get_chunk_size (progress);

		if (left < 1) goto errored;
		while (*C != '\0' && C - CONTENTS < content_length)
		{
			bytes_read = lw_utf8_normalize_chunk (&normalized, C, flags, chunk_size);
			bytes_normalized = strlen(normalized);
			if (normalized != NULL)
			{
				handled_bytes = chunk_handler (normalized, bytes_normalized, chunk_handler_data, &error);
		    g_free (normalized); normalized = NULL;
				if (error != NULL)
				{
          lw_progress_take_error (progress, error);
          error = NULL;
					has_error = TRUE;
					goto errored;
				}
			}
			C += bytes_read;
			left -= bytes_read;
		}

errored:

    return;
}



static GHashTable*
_get_furiganafold_hashtable ()
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


static gchar*
_casefold_character (gchar *character)
{
    //Sanity checks
    if (character == NULL || *character == '\0') return character;

    //Declarations
    gunichar c = -1;
    gunichar lower = -1;
    gchar *n = g_utf8_next_char (character);
    gint bytes = n - character;
    gchar buffer[6] = { 0 };


    //Initializations
    c = g_utf8_get_char (character);
    lower = g_unichar_tolower (c);

    if (c != lower)
    {
      if (g_unichar_to_utf8 (lower, buffer) == bytes)
      {
        strncpy(character, buffer, bytes);
      }
    }
    
    return n;
}


void
lw_utf8_casefold (gchar      *TEXT,
                  gsize       length,
                  LwProgress *progress)
{
    //Sanity checks
    if (TEXT == NULL) return;

    //Declarations
    gint chunk_size = 0;
    gint chunk = 0;
    gchar * c = NULL;
    
    //Initializations
    c = TEXT;
    if (c == NULL) goto errored;
    chunk_size = lw_progress_get_chunk_size (progress);
    if (chunk_size < 1) goto errored;
    if (length < 1)
    {
      length = strlen(TEXT);
    }

    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "%s...", gettext("Folding case"));
      lw_progress_set_total (progress, length);
      lw_progress_set_completed (progress, FALSE);
      lw_progress_set_current (progress, 0);
    }

    {
      gint i = 0;
      while (*c != '\0' && i < length) {
        c = _casefold_character (c);
        i = c - TEXT;
        if (G_UNLIKELY(chunk++ >= chunk_size))
        {
          lw_progress_set_current (progress, i);
          chunk = 0;
        }
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
      lw_progress_set_completed (progress, TRUE);
    }

errored:

    return;
}


static gchar*
_furiganafold_character (gchar *character, GHashTable *conversions)
{
    //Sanity checks
    if (character == NULL || *character == '\0') return character;

    //Declarations
    gunichar c = -1;
    gchar *n = g_utf8_next_char (character);
    gint bytes = n - character;
    gchar buffer[6] = { 0 };

    //Initializations
    c = g_utf8_get_char (character);

    if (g_hash_table_contains (conversions, GUINT_TO_POINTER (c)))
    {
      if (g_unichar_to_utf8 (c, buffer) == bytes)
      {
        strncpy(character, buffer, bytes);
      }
    }
    
    return n;
}


void
lw_utf8_furiganafold (gchar      *TEXT,
                      gsize       length,
                      LwProgress *progress)
{
    //Sanity checks
    if (TEXT == NULL) return;

    //Declarations
    GHashTable *conversions = NULL;
    gint chunk_size = 0;
    gint chunk = 0;
    gchar * c = NULL;
    
    //Initializations
    c = TEXT;
    if (c == NULL) goto errored;
    conversions = _get_furiganafold_hashtable ();
    if (conversions == NULL) goto errored;
    chunk_size = lw_progress_get_chunk_size (progress);
    if (chunk_size < 1) goto errored;
    if (length < 1)
    {
      length = strlen(TEXT);
    }

    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "%s...", gettext("Folding furigana"));
      lw_progress_set_total (progress, length);
      lw_progress_set_completed (progress, FALSE);
      lw_progress_set_current (progress, 0);
    }

    {
      gint i = 0;
      while (*c != '\0' && i < length) {
        c = _furiganafold_character (c, conversions);
        i = c - TEXT;
        if (G_UNLIKELY(chunk++ >= chunk_size))
        {
          lw_progress_set_current (progress, i);
          chunk = 0;
        }
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current (progress, length);
      lw_progress_set_completed (progress, TRUE);
    }

errored:

    if (conversions != NULL) g_hash_table_unref (conversions); conversions = NULL;
}


//!
//! @brief Sanitize an input string
//!
//! This function will check if the input string is a valid utf-8 sequence,
//! it will then normalize this string in the Normalize Form Canonical Composition,
//! then replace the bytes of unprintable unicode glyphe (like control codepoint) with spaces,
//! and finally will remove leading and trailing spaces if asked to.
//!
//! @param text an utf8 encoded string to sanitize
//! @param strip if true remove leading and trailing spaces
//! @return a newly allocated sanitized utf8 encoded string or NULL if text was too.
//!         If the result is non-NULL it must be freed with g_free(). 
//!
gchar*
lw_utf8_sanitize (gchar const * BUFFER)
{
    //Sanity Check
    if (BUFFER == NULL) return NULL;
    
    
    //Declarations
    gchar *copy = NULL;
    gchar const *end = NULL; 

    //Initializations
    copy = g_strdup(BUFFER);
    if (copy == NULL) goto errored;

    //Validate the string as proper utf8
    if (!g_utf8_validate (copy, -1, &end))
    {
      copy[end - copy] = '\0';
    }

    //Clear unprintable characters
    {
      gchar *ptr = copy;
      while (*ptr != '\0')
      {
        gunichar c = g_utf8_get_char (ptr);
        if (!g_unichar_isprint (c))
        {
          gint bytes = g_unichar_to_utf8 (c, NULL);
          memset(ptr, ' ', bytes);
        }
        ptr = g_utf8_next_char (ptr);
      }
    }

    g_strstrip (copy);

errored:

    return copy; 
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



//!
//! @brief Convenience function for seeing if a string is hiragana
//! @param input The string to check
//! @return Returns true if it is in the range
//! @see lw_utf8_is_katakana_str ()
//! @see lw_utf8_is_kanji_str ()
//! @see lw_utf8_is_romaji_str ()
//!
gboolean 
lw_utf8_is_hiragana_str (const char *input)
{
    //Declarations
    gboolean is_consistant;
    gunichar character;
    GUnicodeScript script;
    const char *ptr;

    //Initializations
    is_consistant = TRUE;
    
    //Loop over the string checking for characters inconsistant with the script
    for (ptr = input; *ptr != '\0' && is_consistant; ptr = g_utf8_next_char (ptr))
    {
      character = g_utf8_get_char (ptr);
      script = g_unichar_get_script (character);
      if (script != G_UNICODE_SCRIPT_HIRAGANA &&
          script != G_UNICODE_SCRIPT_COMMON)
        is_consistant = FALSE;
    }

    return is_consistant;
}


//!
//! @brief Convenience function for seeing if a string is katakana
//! @param input The string to check
//! @return Returns true if it is in the range
//! @see lw_utf8_is_hiragana_str ()
//! @see lw_utf8_is_kanji_str ()
//! @see lw_utf8_is_romaji_str ()
//!
gboolean 
lw_utf8_is_katakana_str (const char *input)
{
    //Declarations
    gboolean is_consistant;
    gunichar character;
    GUnicodeScript script;
    const char *ptr;

    //Initializations
    is_consistant = TRUE;
    
    //Loop over the string checking for characters inconsistant with the script
    for (ptr = input; *ptr != '\0' && is_consistant; ptr = g_utf8_next_char (ptr))
    {
      character = g_utf8_get_char (ptr);
      script = g_unichar_get_script (character);
      if (script != G_UNICODE_SCRIPT_KATAKANA &&
          script != G_UNICODE_SCRIPT_COMMON)
        is_consistant = FALSE;
    }

    return is_consistant;
}


//!
//! @brief Convenience function for seeing if a string is furigana
//! @param input The string to check
//! @return Returns true if it is in the range
//! @see lw_utf8_is_hiragana_str ()
//! @see lw_utf8_is_kanji_str ()
//! @see lw_utf8_is_romaji_str ()
//!
gboolean 
lw_utf8_is_furigana_str (const char *input)
{
    return (lw_utf8_is_katakana_str (input) || lw_utf8_is_hiragana_str (input));
}



//!
//! @brief Convenience function for seeing if a string *starts* with kanji
//! @param input The string to check
//! @return Returns true if the function things this is a kanji string
//! @see lw_utf8_is_hiragana_str ()
//! @see lw_utf8_is_katakana_str ()
//! @see lw_utf8_is_romaji_str ()
//!
gboolean 
lw_utf8_is_kanji_ish_str (const char *input)
{
    //Declarations
    gboolean is_consistant;
    gunichar character;
    GUnicodeScript script;
    const char *ptr;

    //Initializations
    is_consistant = TRUE;
    
    //Loop over the string checking for characters inconsistant with the script
    for (ptr = input; *ptr != '\0' && is_consistant; ptr = g_utf8_next_char (ptr))
    {
      character = g_utf8_get_char (ptr);
      script = g_unichar_get_script (character);
      if (script != G_UNICODE_SCRIPT_HAN &&
          script != G_UNICODE_SCRIPT_HIRAGANA &&
          script != G_UNICODE_SCRIPT_KATAKANA &&
          script != G_UNICODE_SCRIPT_COMMON)
        is_consistant = FALSE;
    }

    return is_consistant;
}


gboolean
lw_utf8_string_has_japanese (const gchar *INPUT)
{
    //Declarations
    gboolean found;
    gunichar character;
    GUnicodeScript script;
    const gchar *ptr;

    //Initializations
    found = FALSE;

    //Loop over the string checking for characters inconsistant with the script
    for (ptr = INPUT; *ptr != '\0' && !found; ptr = g_utf8_next_char (ptr))
    {
      character = g_utf8_get_char (ptr);
      script = g_unichar_get_script (character);
      if (script == G_UNICODE_SCRIPT_HAN ||
          script == G_UNICODE_SCRIPT_HIRAGANA ||
          script == G_UNICODE_SCRIPT_KATAKANA)
        found = TRUE;
    }

    return found;
}


//!
//! @brief Convenience function for seeing if a string is kanji
//! @param input The string to check
//! @return Returns true if it is in the range
//! @see lw_utf8_is_hiragana_str ()
//! @see lw_utf8_is_katakana_str ()
//! @see lw_utf8_is_romaji_str ()
//!
gboolean 
lw_utf8_is_kanji_str (const char *input)
{
    //Declarations
    gboolean is_consistant;
    gunichar character;
    GUnicodeScript script;
    const char *ptr;

    //Initializations
    is_consistant = TRUE;
    
    //Loop over the string checking for characters inconsistant with the script
    for (ptr = input; *ptr != '\0' && is_consistant; ptr = g_utf8_next_char (ptr))
    {
      character = g_utf8_get_char (ptr);
      script = g_unichar_get_script (character);
      if (script != G_UNICODE_SCRIPT_HAN &&
          script != G_UNICODE_SCRIPT_COMMON)
        is_consistant = FALSE;
    }

    return is_consistant;
}


//!
//! @brief Convenience function for seeing if a string is romaji
//! @param input The string to check
//! @return Returns true if it is in the range
//! @see lw_utf8_is_hiragana_str ()
//! @see lw_utf8_is_katakana_str ()
//! @see lw_utf8_is_kanji_str ()
//!
gboolean 
lw_utf8_is_romaji_str (const char *input)
{
    //Declarations
    gboolean is_consistant;
    gunichar character;
    GUnicodeScript script;
    const char *ptr;

    //Initializations
    is_consistant = TRUE;
    
    //Loop over the string checking for characters inconsistant with the script
    for (ptr = input; *ptr != '\0' && is_consistant; ptr = g_utf8_next_char (ptr))
    {
      character = g_utf8_get_char (ptr);
      script = g_unichar_get_script (character);
      if (script != G_UNICODE_SCRIPT_LATIN && 
          script != G_UNICODE_SCRIPT_COMMON)
        is_consistant = FALSE;
    }

    return is_consistant;
}


//!
//! @brief Checks if a given string is yojijukugo (a four kanji phrase)
//! @param INPUT
//! @returns Returns TRUE if the text looks to be yojijukugo.  FALSE if it isn't.
//!
gboolean 
lw_utf8_is_yojijukugo_str (const char* INPUT)
{
  return (g_utf8_strlen (INPUT, -1) == 4 && lw_utf8_is_kanji_str (INPUT));
}


//!
//! @brief Shifts kana characters in a specific direction
//! @param input The string to shift
//! @param shift How much to shift by
//! @see lw_utf8_str_shift_hira_to_kata ()
//! @see lw_utf8_str_shift_kata_to_hira ()
//!
void 
lw_utf8_shift_kana_chars_in_str_by (char *input, int shift)
{
    //Setup
    char *input_ptr;
    input_ptr = input;

    char output[strlen(input) + 1];
    char *output_ptr;
    output_ptr = output;

    gunichar unic;
    unic = g_utf8_get_char(input_ptr);

    gint offset = 0;

    //Start the conversion
    while (*input_ptr != '\0')
    {
      if (unic >= 0x3041 && unic <= 0x30ff &&
          unic + shift >= 0x3041 && unic + shift <= 0x30ff &&
          unic != L'ー') {
        offset = g_unichar_to_utf8((unic + shift), output_ptr);
      }
      else {
        offset = g_unichar_to_utf8((unic), output_ptr);
      }
      output_ptr = output_ptr + offset;

      input_ptr = g_utf8_next_char(input_ptr);
      unic = g_utf8_get_char(input_ptr);
    }
    *output_ptr = *input_ptr;

    strcpy(input, output);
}


//!
//! @brief Convenience function to shift hiragana to katakana
//!
//! @param input The string to shift
//! @see lw_utf8_shift_kana_chars_in_str_by ()
//! @see lw_utf8_str_shift_kata_to_hira ()
//!
void 
lw_utf8_str_shift_hira_to_kata (char input[])
{
    lw_utf8_shift_kana_chars_in_str_by (input, (L'ア' - L'あ'));
}


//!
//! @brief Convenience function to shift katakana to hiragana
//!
//! @param input The string to shift
//! @see lw_utf8_shift_kana_chars_in_str_by ()
//! @see lw_utf8_str_shift_hira_to_kata ()
//!
void 
lw_utf8_str_shift_kata_to_hira (char input[])
{
    lw_utf8_shift_kana_chars_in_str_by (input, (L'あ' - L'ア'));
}


//!
//! @brief Gets the next hiragana equivalent char pointer in a string
//!
//! This function returns the hiragana equivalent and skips the romanji equivalent
//! forward in the string.  This function is used for romaji->hiragana conversions.
//!
//! @param input The string to jump around
//! @return where the next hiragana equivalent character would start
//!
const char* 
lw_utf8_next_hira_char_from_roma (const char *input)
{
    const char *input_ptr;
    input_ptr = input;

    int total_n = 0;

    //Cautiously up a character
    if (*input_ptr == 'n')
      total_n++;
    if (*input_ptr != '\0')
      input_ptr++;
    if (*input_ptr == '\0')
      return input_ptr;

    //Loop until we hit a hiragana character ending
    while ( (
            //General conditional for hiragana processing
              *(input_ptr    ) != '\0' &&
              *(input_ptr - 1) != 'a'  &&
              *(input_ptr - 1) != 'i'  &&
              *(input_ptr - 1) != 'u'  &&
              *(input_ptr - 1) != 'e'  &&
              *(input_ptr - 1) != 'o'  &&
              *(input_ptr - 1) != *input_ptr  &&
              *(input_ptr - 1) != '-'  &&
              total_n < 3
            ) ||
            //Special conditional for symbolic n processing
            (
              *(input_ptr    ) != '\0' &&
              *(input_ptr - 1) == 'n'  &&
              *(input_ptr    ) == 'n'  &&
              total_n < 3
            )
          )
    {
      //Update the n count
      if (*(input_ptr - 1) == 'n')
        total_n++;
      else
        total_n = 0;


      if (*(input_ptr - 1) == 'n' &&
              *(input_ptr) != 'a'  &&
              *(input_ptr) != 'i'  &&
              *(input_ptr) != 'u'  &&
              *(input_ptr) != 'e'  &&
              *(input_ptr) != 'o'  &&
              *(input_ptr) != 'y'
         )
        break;

      //Increment
      input_ptr++;
    }
    if (*(input_ptr - 1) == 'n' &&
            *(input_ptr) == 'n'
       )
      input_ptr++;


    return input_ptr;
}


//!
//! @brief Converts a romaji string to hiragana.
//! @param input The source romaji string
//! @param output The string to write the hiragana equivalent to
//! @return Returns null on error/end
//!
gchar* 
lw_utf8_roma_char_to_hira (const gchar *input, gchar *output)
{
    //Set up the input pointer
    const char *input_ptr;
    input_ptr = input;

    //Make sure the output pointer is initialized
    output[0] = '\0';
    
    //Set up the buffer variables
    char buffer[] = "           ";
    char *buffer_ptr = buffer;

    //Copy the next hiragana char written in romaji to the buffer
    while ( 
            (
              *input_ptr != '\0' &&
              *input_ptr != 'a'  &&
              *input_ptr != 'i'  &&
              *input_ptr != 'u'  &&
              *input_ptr != 'e'  &&
              *input_ptr != 'o'  &&
              *input_ptr != '-'  && 
              *input_ptr != *(input_ptr + 1)  && 
              *(buffer_ptr + 1) == ' '                
            ) 
          )
    {
      *buffer_ptr = *input_ptr;
      input_ptr++;
      buffer_ptr++;
    }
    *buffer_ptr = *input_ptr;
    buffer_ptr++;
    *buffer_ptr = '\0';

    //HACK!!!!!!!!!!!!!!!!
    if (buffer[0] == 'n' &&
              buffer[1] != 'a'  &&
              buffer[1] != 'i'  &&
              buffer[1] != 'u'  &&
              buffer[1] != 'e'  &&
              buffer[1] != 'o'  &&
              buffer[1] != 'y'
       )
    {
       buffer[1] = '\0';
    }
    else if ( buffer[0] != 'a'  &&
              buffer[0] != 'i'  &&
              buffer[0] != 'u'  &&
              buffer[0] != 'e'  &&
              buffer[0] != 'o'  &&
              buffer[0] != 'n'  &&
              strlen(input) == 1
            )
    {
       return NULL;
    }
    //HACK!!!!!!!!!!!!!!!!


    //Reset the buffer pointer
    buffer_ptr = buffer;


    //
    //Start main lookup for conversion
    //

    if (strcmp(buffer_ptr, "n") == 0)
       strcpy(output, "ん");


    else if (strlen(buffer_ptr) == 1 &&
             buffer_ptr[0] != 'a'    &&
             buffer_ptr[0] != 'i'    &&
             buffer_ptr[0] != 'u'    &&
             buffer_ptr[0] != 'e'    &&
             buffer_ptr[0] != 'o'    &&
             buffer_ptr[0] != '-'    &&
             buffer_ptr[0] != 'y'    &&
             input_ptr[1] != '\0'      )
       strcpy(output, "っ");

    else if (strcmp(buffer_ptr, "a") == 0)
       strcpy(output, "あ");
    else if (strcmp(buffer_ptr, "i") == 0)
       strcpy(output, "い");
    else if (strcmp(buffer_ptr, "u") == 0)
       strcpy(output, "う");
    else if (strcmp(buffer_ptr, "e") == 0)
       strcpy(output, "え");
    else if (strcmp(buffer_ptr, "o") == 0)
       strcpy(output, "お");


    else if (strcmp(buffer_ptr, "ka") == 0 || strcmp(buffer_ptr, "ca") == 0)
       strcpy(output, "か");
    else if (strcmp(buffer_ptr, "ki") == 0 || strcmp(buffer_ptr, "ci") == 0)
       strcpy(output, "き");
    else if (strcmp(buffer_ptr, "ku") == 0 || strcmp(buffer_ptr, "cu") == 0)
       strcpy(output, "く");
    else if (strcmp(buffer_ptr, "ke") == 0 || strcmp(buffer_ptr, "ce") == 0)
       strcpy(output, "け");
    else if (strcmp(buffer_ptr, "ko") == 0 || strcmp(buffer_ptr, "co") == 0)
       strcpy(output, "こ");

    else if (strcmp(buffer_ptr, "kya") == 0 || strcmp(buffer_ptr, "cya") == 0)
       strcpy(output, "きゃ");
    else if (strcmp(buffer_ptr, "kyu") == 0 || strcmp(buffer_ptr, "cyu") == 0)
       strcpy(output, "きゅ");
    else if (strcmp(buffer_ptr, "kyo") == 0 || strcmp(buffer_ptr, "cyo") == 0)
       strcpy(output, "きょ");

    else if (strcmp(buffer_ptr, "ga") == 0)
       strcpy(output, "が");
    else if (strcmp(buffer_ptr, "gi") == 0)
       strcpy(output, "ぎ");
    else if (strcmp(buffer_ptr, "gu") == 0)
       strcpy(output, "ぐ");
    else if (strcmp(buffer_ptr, "ge") == 0)
       strcpy(output, "げ");
    else if (strcmp(buffer_ptr, "go") == 0)
       strcpy(output, "ご");

    else if (strcmp(buffer_ptr, "gya") == 0)
       strcpy(output, "ぎゃ");
    else if (strcmp(buffer_ptr, "gyu") == 0)
       strcpy(output, "ぎゅ");
    else if (strcmp(buffer_ptr, "gyo") == 0)
       strcpy(output, "ぎょ");


    else if (strcmp(buffer_ptr, "sa") == 0)
       strcpy(output, "さ");
    else if (strcmp(buffer_ptr, "si") == 0 || strcmp(buffer_ptr, "shi") == 0)
       strcpy(output, "し");
    else if (strcmp(buffer_ptr, "su") == 0)
       strcpy(output, "す");
    else if (strcmp(buffer_ptr, "se") == 0)
       strcpy(output, "せ");
    else if (strcmp(buffer_ptr, "so") == 0)
       strcpy(output, "そ");

    else if (strcmp(buffer_ptr, "sya") == 0 || strcmp(buffer_ptr, "sha") == 0)
       strcpy(output, "しゃ");
    else if (strcmp(buffer_ptr, "syu") == 0 || strcmp(buffer_ptr, "shu") == 0)
       strcpy(output, "しゅ");
    else if (strcmp(buffer_ptr, "syo") == 0 || strcmp(buffer_ptr, "sho") == 0)
       strcpy(output, "しょ");

    else if (strcmp(buffer_ptr, "za") == 0)
       strcpy(output, "ざ");
    else if (strcmp(buffer_ptr, "zi") == 0 || strcmp(buffer_ptr, "ji") == 0)
       strcpy(output, "じ");
    else if (strcmp(buffer_ptr, "zu") == 0)
       strcpy(output, "ず");
    else if (strcmp(buffer_ptr, "ze") == 0)
       strcpy(output, "ぜ");
    else if (strcmp(buffer_ptr, "zo") == 0)
       strcpy(output, "ぞ");

    else if (strcmp(buffer_ptr, "zya") == 0 || strcmp(buffer_ptr, "jya") == 0
                                            || strcmp(buffer_ptr, "ja" ) == 0 )
       strcpy(output, "じゃ");
    else if (strcmp(buffer_ptr, "zyu") == 0 || strcmp(buffer_ptr, "jyu") == 0
                                            || strcmp(buffer_ptr, "ju" ) == 0 )
       strcpy(output, "じゅ");
    else if (strcmp(buffer_ptr, "zyo") == 0 || strcmp(buffer_ptr, "jyo") == 0
                                            || strcmp(buffer_ptr, "jo" ) == 0 )
       strcpy(output, "じょ");


    else if (strcmp(buffer_ptr, "ta") == 0)
       strcpy(output, "た");
    else if (strcmp(buffer_ptr, "ti") == 0 || strcmp(buffer_ptr, "chi") == 0)
       strcpy(output, "ち");
    else if (strcmp(buffer_ptr, "tu") == 0 || strcmp(buffer_ptr, "tsu") == 0)
       strcpy(output, "つ");
    else if (strcmp(buffer_ptr, "te") == 0)
       strcpy(output, "て");
    else if (strcmp(buffer_ptr, "to") == 0)
       strcpy(output, "と");

    else if (strcmp(buffer_ptr, "tya") == 0 || strcmp(buffer_ptr, "cha") == 0)
       strcpy(output, "ちゃ");
    else if (strcmp(buffer_ptr, "tyu") == 0 || strcmp(buffer_ptr, "chu") == 0)
       strcpy(output, "ちゅ");
    else if (strcmp(buffer_ptr, "tyo") == 0 || strcmp(buffer_ptr, "cho") == 0)
       strcpy(output, "ちょ");

    else if (strcmp(buffer_ptr, "da") == 0)
       strcpy(output, "だ");
    else if (strcmp(buffer_ptr, "di") == 0)
       strcpy(output, "ぢ");
    else if (strcmp(buffer_ptr, "du") == 0 || strcmp(buffer_ptr, "dsu") == 0)
       strcpy(output, "づ");
    else if (strcmp(buffer_ptr, "de") == 0)
       strcpy(output, "で");
    else if (strcmp(buffer_ptr, "do") == 0)
       strcpy(output, "ど");

    else if (strcmp(buffer_ptr, "dya") == 0)
       strcpy(output, "ぢゃ");
    else if (strcmp(buffer_ptr, "dyu") == 0)
       strcpy(output, "ぢゅ");
    else if (strcmp(buffer_ptr, "dyo") == 0)
       strcpy(output, "ぢょ");


    else if (strcmp(buffer_ptr, "na") == 0)
       strcpy(output, "な");
    else if (strcmp(buffer_ptr, "ni") == 0)
       strcpy(output, "に");
    else if (strcmp(buffer_ptr, "nu") == 0)
       strcpy(output, "ぬ");
    else if (strcmp(buffer_ptr, "ne") == 0)
       strcpy(output, "ね");
    else if (strcmp(buffer_ptr, "no") == 0)
       strcpy(output, "の");

    else if (strcmp(buffer_ptr, "nya") == 0)
       strcpy(output, "にゃ");
    else if (strcmp(buffer_ptr, "nyu") == 0)
       strcpy(output, "にゅ");
    else if (strcmp(buffer_ptr, "nyo") == 0)
       strcpy(output, "にょ");


    else if (strcmp(buffer_ptr, "ha") == 0)
       strcpy(output, "は");
    else if (strcmp(buffer_ptr, "hi") == 0)
       strcpy(output, "ひ");
    else if (strcmp(buffer_ptr, "hu") == 0 || strcmp(buffer_ptr, "fu") == 0)
       strcpy(output, "ふ");
    else if (strcmp(buffer_ptr, "he") == 0)
       strcpy(output, "へ");
    else if (strcmp(buffer_ptr, "ho") == 0)
       strcpy(output, "ほ");

    else if (strcmp(buffer_ptr, "hya") == 0)
       strcpy(output, "ひゃ");
    else if (strcmp(buffer_ptr, "hyu") == 0)
       strcpy(output, "ひゅ");
    else if (strcmp(buffer_ptr, "hyo") == 0)
       strcpy(output, "ひょ");
   
    else if (strcmp(buffer_ptr, "ba") == 0)
       strcpy(output, "ば");
    else if (strcmp(buffer_ptr, "bi") == 0)
       strcpy(output, "び");
    else if (strcmp(buffer_ptr, "bu") == 0)
       strcpy(output, "ぶ");
    else if (strcmp(buffer_ptr, "be") == 0)
       strcpy(output, "べ");
    else if (strcmp(buffer_ptr, "bo") == 0)
       strcpy(output, "ぼ");

    else if (strcmp(buffer_ptr, "bya") == 0)
       strcpy(output, "びゃ");
    else if (strcmp(buffer_ptr, "byu") == 0)
       strcpy(output, "びゅ");
    else if (strcmp(buffer_ptr, "byo") == 0)
       strcpy(output, "びょ");

    else if (strcmp(buffer_ptr, "pa") == 0)
       strcpy(output, "ぱ");
    else if (strcmp(buffer_ptr, "pi") == 0)
       strcpy(output, "ぴ");
    else if (strcmp(buffer_ptr, "pu") == 0)
       strcpy(output, "ぷ");
    else if (strcmp(buffer_ptr, "pe") == 0)
       strcpy(output, "ぺ");
    else if (strcmp(buffer_ptr, "po") == 0)
       strcpy(output, "ぽ");

    else if (strcmp(buffer_ptr, "pya") == 0)
       strcpy(output, "ぴゃ");
    else if (strcmp(buffer_ptr, "pyu") == 0)
       strcpy(output, "ぴゅ");
    else if (strcmp(buffer_ptr, "pyo") == 0)
       strcpy(output, "ぴょ");


    else if (strcmp(buffer_ptr, "ma") == 0)
       strcpy(output, "ま");
    else if (strcmp(buffer_ptr, "mi") == 0)
       strcpy(output, "み");
    else if (strcmp(buffer_ptr, "mu") == 0)
       strcpy(output, "む");
    else if (strcmp(buffer_ptr, "me") == 0)
       strcpy(output, "め");
    else if (strcmp(buffer_ptr, "mo") == 0)
       strcpy(output, "も");

    else if (strcmp(buffer_ptr, "mya") == 0)
       strcpy(output, "みゃ");
    else if (strcmp(buffer_ptr, "myu") == 0)
       strcpy(output, "みゅ");
    else if (strcmp(buffer_ptr, "myo") == 0)
       strcpy(output, "みょ");


    else if (strcmp(buffer_ptr, "ya") == 0)
       strcpy(output, "や");
    else if (strcmp(buffer_ptr, "yu") == 0)
       strcpy(output, "ゆ");
    else if (strcmp(buffer_ptr, "yo") == 0)
       strcpy(output, "よ");


    else if (strcmp(buffer_ptr, "ra") == 0 || strcmp(buffer_ptr, "la") == 0)
       strcpy(output, "ら");
    else if (strcmp(buffer_ptr, "ri") == 0 || strcmp(buffer_ptr, "li") == 0)
       strcpy(output, "り");
    else if (strcmp(buffer_ptr, "ru") == 0 || strcmp(buffer_ptr, "lu") == 0)
       strcpy(output, "る");
    else if (strcmp(buffer_ptr, "re") == 0 || strcmp(buffer_ptr, "le") == 0)
       strcpy(output, "れ");
    else if (strcmp(buffer_ptr, "ro") == 0 || strcmp(buffer_ptr, "lo") == 0)
       strcpy(output, "ろ");

    else if (strcmp(buffer_ptr, "rya") == 0 || strcmp(buffer_ptr, "lya") == 0)
       strcpy(output, "りゃ");
    else if (strcmp(buffer_ptr, "ryu") == 0 || strcmp(buffer_ptr, "lyu") == 0)
       strcpy(output, "りゅ");
    else if (strcmp(buffer_ptr, "ryo") == 0 || strcmp(buffer_ptr, "lyo") == 0)
       strcpy(output, "りょ");


    else if (strcmp(buffer_ptr, "wa") == 0)
       strcpy(output, "わ");
    else if (strcmp(buffer_ptr, "wi") == 0)
       strcpy(output, "うぃ");
    else if (strcmp(buffer_ptr, "we") == 0)
       strcpy(output, "うぇ");
    else if (strcmp(buffer_ptr, "wo") == 0)
       strcpy(output, "を");

    else if (strcmp(buffer_ptr, "va") == 0)
       strcpy(output, "う゛ぁ");
    else if (strcmp(buffer_ptr, "vi") == 0)
       strcpy(output, "う゛ぃ");
    else if (strcmp(buffer_ptr, "ve") == 0)
       strcpy(output, "う゛ぇ");
    else if (strcmp(buffer_ptr, "vo") == 0)
       strcpy(output, "う゛ぉ");


    else if (strcmp(buffer_ptr, "xa") == 0)
       strcpy(output, "ぁ");
    else if (strcmp(buffer_ptr, "xi") == 0)
       strcpy(output, "ぃ");
    else if (strcmp(buffer_ptr, "xu") == 0)
       strcpy(output, "ぅ");
    else if (strcmp(buffer_ptr, "xe") == 0)
       strcpy(output, "ぇ");
    else if (strcmp(buffer_ptr, "xo") == 0)
       strcpy(output, "ぉ");


    else if (strcmp(buffer_ptr, "fa") == 0)
       strcpy(output, "ふぁ");
    else if (strcmp(buffer_ptr, "fi") == 0)
       strcpy(output, "ふぃ");
    else if (strcmp(buffer_ptr, "fe") == 0)
       strcpy(output, "ふぇ");
    else if (strcmp(buffer_ptr, "fo") == 0)
       strcpy(output, "ふぉ");
   

    else if (strcmp(buffer_ptr, "-") == 0)
       strcpy(output, "ー");

    else
    {
      input_ptr = NULL;
      return NULL;
    }

    return output;
}


//!
//! @brief Convenience function to convert romaji to hiragana
//!
//! @param input The string to shift.
//! @param output the string to output the changes to.
//! @param max The max length of the string to output to.
//! @see lw_utf8_shift_kana_chars_in_str_by ()
//! @see lw_utf8_str_shift_hira_to_kata ()
//!
gboolean 
lw_utf8_str_roma_to_hira (const gchar* input, gchar* output, gint max)
{
    //Sanity checks
    g_return_val_if_fail (input != NULL, FALSE);
    g_return_val_if_fail (output != NULL, FALSE);

    //Declarations
    const gchar *input_ptr;
    gchar *kana_ptr;
    gint leftover;

    //Initializations
    input_ptr = input;
    kana_ptr = output;
    *kana_ptr = '\0';
    leftover = max;

    //Try converting to hiragana
    while (leftover-- > 0)
    {
      kana_ptr = lw_utf8_roma_char_to_hira (input_ptr, kana_ptr);
      if (kana_ptr == NULL || input_ptr == NULL)
        break;
      input_ptr = lw_utf8_next_hira_char_from_roma (input_ptr);
      if (kana_ptr == NULL || input_ptr == NULL)
        break;

      kana_ptr = &kana_ptr[strlen(kana_ptr)];
    }

    return (input_ptr != NULL && strlen (input_ptr) == 0);
}


static gboolean
lw_utf8_script_changed (GUnicodeScript p, GUnicodeScript n, gboolean split_kanji_furigana)
{
    //Declarations
    gboolean has_common;
    gboolean has_changed;
    gboolean is_japanese_p;
    gboolean is_japanese_n;
    gboolean is_japanese_change;
    
    //Initializations;
    has_common = (p == G_UNICODE_SCRIPT_INVALID_CODE ||	n == G_UNICODE_SCRIPT_COMMON || p == G_UNICODE_SCRIPT_COMMON);
    has_changed = (p != n && !has_common);
    is_japanese_p = (p == G_UNICODE_SCRIPT_HAN || p == G_UNICODE_SCRIPT_HIRAGANA || p == G_UNICODE_SCRIPT_KATAKANA);
    is_japanese_n = (n == G_UNICODE_SCRIPT_HAN || n == G_UNICODE_SCRIPT_HIRAGANA || n == G_UNICODE_SCRIPT_KATAKANA);
    is_japanese_change = (is_japanese_p && is_japanese_n  && p != n);


    if (is_japanese_change) return (split_kanji_furigana);
    else return (has_changed);
}


gchar*
lw_utf8_delimit_script_changes (const gchar *DELIMITOR, const gchar* TEXT, gboolean split_kanji_furigana)
{
    //Sanity check
    g_return_val_if_fail (DELIMITOR != NULL && TEXT != NULL, NULL);

    //Declarations
    gchar *buffer = NULL;
    gint count = 0;
    const gchar *source_ptr = NULL;
    gchar *target_ptr = NULL;
    GUnicodeScript this_script = 0, previous_script = 0;
    gunichar c = 0;
    gboolean script_changed = 0;
    gint delimitor_length = 0;

    //Initializations
    count = 0;
    delimitor_length = strlen (DELIMITOR);
    this_script = previous_script = G_UNICODE_SCRIPT_INVALID_CODE;
    for (source_ptr = TEXT; *source_ptr != '\0'; source_ptr = g_utf8_next_char (source_ptr))
    {
      c = g_utf8_get_char (source_ptr);
      this_script = g_unichar_get_script (c);
      script_changed = lw_utf8_script_changed (previous_script, this_script, split_kanji_furigana);

      if (script_changed)
      {
				count++;
      }

      previous_script = this_script;
    }

    buffer = g_new (gchar, strlen(TEXT) + (delimitor_length * count) + 1);
		if (buffer != NULL)
		{
      target_ptr = buffer;
			*buffer = '\0';
			this_script = previous_script = G_UNICODE_SCRIPT_INVALID_CODE;
			for (source_ptr = TEXT; *source_ptr != '\0'; source_ptr = g_utf8_next_char (source_ptr))
			{
				c = g_utf8_get_char (source_ptr);
				this_script = g_unichar_get_script (c);
        script_changed = lw_utf8_script_changed (previous_script, this_script, split_kanji_furigana);

        if (script_changed)
				{
					strcpy(target_ptr, DELIMITOR);
					target_ptr += delimitor_length;
				}
        target_ptr += g_unichar_to_utf8 (c, target_ptr);
        *target_ptr = '\0';

				previous_script = this_script;
			}
		}

    return buffer;
}


gchar*
lw_utf8_delimit_whitespace (const gchar *DELIMITOR, const gchar* TEXT)
{
    //Sanity check
    g_return_val_if_fail (DELIMITOR != NULL && TEXT != NULL, NULL);

    //Declarations
    gchar *buffer;
    gint count;
    const gchar *source_ptr;
    gchar *target_ptr;
    gunichar c;
    gint delimitor_length;
    gboolean inserted_delimitor;
    gboolean is_whitespace;

    //Initializations
    count = 0;
    delimitor_length = strlen(DELIMITOR);
    inserted_delimitor = FALSE;
    for (source_ptr = TEXT; *source_ptr != '\0'; source_ptr = g_utf8_next_char (source_ptr))
    {
      c = g_utf8_get_char (source_ptr);
      is_whitespace = g_unichar_isspace(c);

      if (is_whitespace)
      {
        count++;
      }
    }
    buffer = g_new (gchar, strlen(TEXT) + (delimitor_length * count) + 1);
    target_ptr = buffer;

    //Create the delimited string
    if (buffer != NULL)
    {
      for (source_ptr = TEXT; *source_ptr != '\0'; source_ptr = g_utf8_next_char (source_ptr))
      {
        c = g_utf8_get_char (source_ptr);
        is_whitespace = g_unichar_isspace(c);

        if (is_whitespace && !inserted_delimitor)
        {
          strcpy(target_ptr, DELIMITOR);
          target_ptr += delimitor_length;
          inserted_delimitor = TRUE;
        }
        else if (!is_whitespace)
        {
          target_ptr += g_unichar_to_utf8 (c, target_ptr);
          *target_ptr = '\0';
          inserted_delimitor = FALSE;
        }
      }
    }

    return buffer;
}


gchar*
lw_utf8_delimit_radicals (const gchar *DELIMITOR, const gchar* TEXT)
{
    //Sanity check
    g_return_val_if_fail (DELIMITOR != NULL && TEXT != NULL, NULL);

    //Declarations
    gchar *buffer;
    gint count;
    const gchar *source_ptr;
    gchar *target_ptr;
    gunichar c;
    gint delimitor_length;
    GUnicodeScript script;
    GUnicodeScript previous_script;

    //Initializations
    count = 0;
    delimitor_length = strlen(DELIMITOR);
    previous_script = G_UNICODE_SCRIPT_INVALID_CODE;

    for (source_ptr = TEXT; *source_ptr != '\0'; source_ptr = g_utf8_next_char (source_ptr))
    {
      c = g_utf8_get_char (source_ptr);
      script = g_unichar_get_script (c);

      if (previous_script == G_UNICODE_SCRIPT_HAN && script == previous_script)
      {
        count++;
      }
      previous_script = script;
    }

    buffer = g_new (gchar, strlen(TEXT) + (delimitor_length * count) + 1);
    target_ptr = buffer;
    previous_script = G_UNICODE_SCRIPT_INVALID_CODE;

    //Create the delimited string
    if (buffer != NULL)
    {
      for (source_ptr = TEXT; *source_ptr != '\0'; source_ptr = g_utf8_next_char (source_ptr))
      {
        c = g_utf8_get_char (source_ptr);
        script = g_unichar_get_script (c);

        if (previous_script == G_UNICODE_SCRIPT_HAN && script == previous_script)
        {
          strcpy(target_ptr, DELIMITOR);
          target_ptr += delimitor_length;
        }

        target_ptr += g_unichar_to_utf8 (c, target_ptr);
        *target_ptr = '\0';

        previous_script = script;
      }
    }

    return buffer;
}


gsize
lw_utf8_replace_linebreaks_with_nullcharacter (gchar      *CONTENTS,
                                               gsize       content_length,
                                               gsize      *max_line_length,
                                               LwProgress *progress)
{
    g_return_if_fail (CONTENTS != NULL);

    //Declarations    
    gint num_lines = 0;
    gsize chunk_size = 0;

    //Initializations
    if (content_length < 1) content_length = strlen(CONTENTS);
    chunk_size = lw_progress_get_chunk_size (progress);

    if (progress != NULL)
    {
      if (lw_progress_get_secondary_message (progress) == NULL)
      {
        lw_progress_set_secondary_message_printf (progress, "Delimiting lines...");
        lw_progress_set_completed (progress, FALSE);
        lw_progress_set_total (progress, content_length);
        lw_progress_set_current (progress, 0);
      }
    }

    {
      gchar *c = NULL;
      gchar *p = NULL;
      gchar *e = NULL;
      gchar *n = NULL;
      gsize chunk = 0;

      n = p = c = CONTENTS;
      e = c + content_length;

      while (*c != '\0' && c < e)
      {
        while (*c == '\n' && c < e) 
        {
          if (max_line_length != NULL)
          {
            gsize content_length = c - p;
            if (content_length > *max_line_length) *max_line_length = content_length;
          }
          n = lw_utf8_set_null_next_char (c);
          chunk += n - c;
          p = c = n;
          num_lines++;
        }
        while (*c != '\0' && *c != '\n' && c < e)
        {
          n = g_utf8_next_char (c);
          chunk += n - c;
          c = n;
        }
        if (progress != NULL && chunk >= chunk_size)
        {
          lw_progress_set_current (progress, c - CONTENTS);
          chunk = 0;
        }
      }
    }

    if (progress != NULL)
    {
      lw_progress_set_current, (progress, content_length);
      lw_progress_set_completed (progress, TRUE);
    }

    return num_lines;
}


GRegex*
lw_regex_new (const gchar *PATTERN, const gchar *EXPRESSION, GError **error)
{
    //Declarations
    GRegex *regex;
    gchar *expression;

    //Initializations
    expression = g_strdup_printf (PATTERN, EXPRESSION);
    regex = NULL;

    if (expression != NULL)
    {
      regex = g_regex_new (expression,  (G_REGEX_CASELESS | G_REGEX_OPTIMIZE), 0, error);
      g_free (expression); expression = NULL;
    }

    return regex;
}


//converts "%s Copy %d" to "(?P<text>.+) Copy (?P<number>[0-9]+)"
//, accessable with (?P=number) or (?P=text>)
gchar*
lw_utf8_convert_printf_pattern_to_regex_pattern (const gchar *PATTERN)
{
    //Sanity checks
    g_return_val_if_fail (PATTERN != NULL, NULL);

    //Declarations
    GRegex *number_pattern = NULL;
    GRegex *string_pattern = NULL;
    gchar *pattern = NULL;

    //Initializations
    number_pattern = g_regex_new ("%d", 0, 0, NULL);
    if (number_pattern == NULL) goto errored;
    string_pattern = g_regex_new ("%s", 0, 0, NULL);
    if (string_pattern == NULL) goto errored;
    pattern = g_strdup (PATTERN);
    if (pattern == NULL) goto errored;

    {
      gchar *replaced = NULL;
      replaced = g_regex_replace_literal (number_pattern, pattern, -1, 0, "(?P<number>[0-9]+)", 0, NULL);
      if (replaced != NULL)
      {
        g_free (pattern);
        pattern = replaced;
        replaced = NULL;
      }
    }
    {
      gchar *replaced = NULL;
      replaced = g_regex_replace_literal (string_pattern, pattern, -1, 0, "(?P<text>.+)", 0, NULL);
      if (replaced != NULL)
      {
        g_free (pattern);
        pattern = replaced;
        replaced = NULL;
      }
    }

    {
      gchar *anchored = NULL;
      anchored = g_strdup_printf ("^%s$", pattern);
      if (anchored != NULL)
      {
        g_free (pattern);
        pattern = anchored;
        anchored = NULL;
      }
    }

errored:

    if (number_pattern != NULL) g_regex_unref (number_pattern); number_pattern = NULL;
    if (string_pattern != NULL) g_regex_unref (string_pattern); string_pattern = NULL;

    return pattern;
}


gint
lw_utf8_count_lines (gchar *buffer)
{
    if (buffer == NULL) return 0;

    //Declarations
    gchar *c = NULL;
    gint count = 0;

    //Initializations
    c = buffer;

    while (*c != '\0')
    {
      if (*c == '\n') count++;
      c = g_utf8_next_char (c);
    }

    return count;
}


gchar**
lw_utf8_split_lines (gchar *buffer, gint *num_lines)
{
    //Sanity checks
    if (buffer == NULL) return 0;

    //Declarations
    gchar *c = NULL;
    gchar **lines = NULL;
    gint count = 0;

    //Initializations
    c = buffer;
    count = lw_utf8_count_lines (c);
    lines = g_new (gchar*, count + 1);
    if (lines == NULL) goto errored;

    //Make the lines separate strings
    {
      gint i = 0;
      while (*c != '\0' && i < count)
      {
        while (*c != '\0' && *c == '\n') c = lw_utf8_set_null_next_char (c);
        if (*c != '\0') lines[i++] = c;
        while (*c != '\0' && *c != '\n') c = g_utf8_next_char (c);
      }
      lines[i++] = NULL;
    }

errored:

    if (num_lines != NULL) *num_lines = count;

    return lines;
}


gboolean
lw_utf8_isescaped (gchar const * TEXT,
                   gchar const * CHAR_PTR)
{
    gboolean is_escaped = FALSE;

    while (CHAR_PTR > TEXT)
    {
      if (*(CHAR_PTR-1) == '\\')
      {
        is_escaped = !is_escaped;
      }
      CHAR_PTR--;
    }
    return is_escaped;
}

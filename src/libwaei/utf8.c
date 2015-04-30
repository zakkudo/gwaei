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

/**
 * SECTION:utf8
 * @short_description: UTF-8 utility methods
 * @title: UTF-8 
 *
 * Utility methods for interacting with utf8 strings and converting encoding.
 */

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


/**
 * lw_utf8_normalize:
 * @TEXT: A string to normalize.  A copy will be made
 * @length: Length of the string in bytes or -1 to have it calculated
 * @flags: Flags for the type of requested normalization
 *
 * Normalizes a string and returns a copy.  This method should be used before
 * doing comparisons.  It also features normalizing katakana to hiragana
 * (through the furiganafold option) and normalizing latin characters to
 * lowercase (through  the casefold option.)
 *
 * Returns: A newly allocated normalized string that should be freed with g_free()
 */
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
    GHashTable * table = NULL;
    GHashTable * table_out = NULL;
    static const gchar * KATAKANA = "ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶヽヾヿ";
    static const gchar * HIRAGANA = "ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖゝゞゟ";
    gchar const * K = NULL;
    gchar const * H = NULL;

    //Initializations
    table = g_hash_table_new (g_direct_hash, g_direct_equal);
    if (table == NULL) goto errored;
    K = KATAKANA;
    H = HIRAGANA;

    while (*H != '\0' && *K != '\0')
    {
      gpointer key = GUINT_TO_POINTER (g_utf8_get_char (K));
      gpointer value = GUINT_TO_POINTER (g_utf8_get_char (H));

      g_hash_table_insert (table, key, value);

      H = g_utf8_next_char (H);
      K = g_utf8_next_char (K);
    }

    table_out = table;
    table = NULL;

errored:

    if (table != NULL) g_hash_table_unref (table);
    table = NULL;

    return table_out;    
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


/**
 * lw_utf8_casefold:
 * @TEXT: A string to casefold inline.
 * @length: Length of the string in bytes or -1 to have it calculated
 * @progress: An #LwProgress to track progress
 *
 * Folds the case to lower case so that case insensitive searches can be
 * accomplished.
 */
void
lw_utf8_casefold (gchar      *TEXT,
                  gssize      length,
                  LwProgress *progress)
{
    //Sanity checks
    if (TEXT == NULL) return;

    //Declarations
    gint chunk_size = -1;
    gint chunk = 0;
    gchar * c = NULL;
    
    //Initializations
    c = TEXT;
    if (c == NULL) goto errored;
    if (progress != NULL)
    {
      chunk_size = lw_progress_get_chunk_size (progress);
      if (chunk_size < 1) goto errored;
    }
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
        if (G_UNLIKELY(chunk++ >= chunk_size) && chunk_size > -1)
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
_furiganafold_character (gchar *c, GHashTable *conversions)
{
    //Sanity checks
    if (c == NULL || *c == '\0') return c;

    //Declarations
    gunichar unichar = -1;
    gchar * n = NULL;
    gunichar conversion = NULL;
    gint len = 0;
    gchar buffer[6] = { 0 };

    //Initializations
    n = g_utf8_next_char (c);
    unichar = g_utf8_get_char (c);

    if (g_hash_table_contains (conversions, GUINT_TO_POINTER (unichar)))
    {
      conversion = (gunichar) GPOINTER_TO_UINT (g_hash_table_lookup (conversions, GUINT_TO_POINTER (unichar)));
      len = g_unichar_to_utf8 (conversion, buffer);
      if (len <= n - c)
      {
        strncpy (c, buffer, len);
      }
    }
    
    return n;
}


/**
 * lw_utf8_furiganafold:
 * @TEXT: A string to furiganafold inline.
 * @length: Length of the string in bytes or -1 to have it calculated
 * @progress: An #LwProgress to track progress
 *
 * Folds katakana to hiragana characters so furigana insensitive comparisons
 * can be made.
 */
void
lw_utf8_furiganafold (gchar      *TEXT,
                      gssize      length,
                      LwProgress *progress)
{
    //Sanity checks
    if (TEXT == NULL) return;

    //Declarations
    GHashTable *conversions = NULL;
    gint chunk_size = -1;
    gint chunk = 0;
    gchar * c = NULL;
    
    //Initializations
    c = TEXT;
    if (c == NULL) goto errored;
    conversions = _get_furiganafold_hashtable ();
    if (conversions == NULL) goto errored;
    if (progress != NULL)
    {
      chunk_size = lw_progress_get_chunk_size (progress);
      if (chunk_size < 1) goto errored;
    }
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
        if (G_UNLIKELY(chunk++ > chunk_size) && chunk_size > -1 && progress != NULL)
        {
          if (lw_progress_should_abort (progress)) goto errored;
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


/**
 * lw_utf8_sanitize:
 * @TEXT: A string to sanitize
 *
 * Sanitizes a string as utf8, replacing invalid characters with spaces.
 * 
 * Returns: A sanitized string that needs to be freed with g_free()
 */
gchar *
lw_utf8_sanitize (gchar const * TEXT)
{
    //Sanity Check
    if (TEXT == NULL) return NULL;
    
    
    //Declarations
    gchar *copy = NULL;
    gchar const *end = NULL; 

    //Initializations
    copy = g_strdup (TEXT);
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


gboolean 
lw_utf8_is_furigana_str (const char *input)
{
    return (lw_utf8_is_katakana_str (input) || lw_utf8_is_hiragana_str (input));
}


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


gboolean 
lw_utf8_is_yojijukugo_str (const char* INPUT)
{
  return (g_utf8_strlen (INPUT, -1) == 4 && lw_utf8_is_kanji_str (INPUT));
}


gchar *
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

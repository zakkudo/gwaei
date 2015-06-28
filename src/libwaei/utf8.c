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


/**
 * lw_utf8_error_quark:
 *
 * Returns: a #GQuark representing a standard lw utf8 domain error
 */
GQuark
lw_utf8_error_quark ()
{
    return g_quark_from_static_string ("lw-utf8-error");
}


/**
 * lw_utf8_set_null_next_char:
 * @text: (transfer none): A pointer to a character in a string to nullify
 * Returns: (transfer none): The next character in the string
 */
gchar *
lw_utf8_set_null_next_char (gchar * text)
{
    //Declarations
    gchar * c = text;

    if (c != NULL && *c != '\0')
    {
      gchar * next = g_utf8_next_char (c);
      *c = '\0';
      c = next;
    }

    return c;
}


gint *
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


/**
 * lw_utf8flag_get_type:
 * Returns: The #GType of a libwaei utf8 flags
 */
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


/**
 * lw_utf8flag_clean:
 * @flags: A set of #LwUtf8Flags to clean
 * 
 * Removes invalid options from the flag mask.
 *
 * Returns: Cleaned #LwUtf8Flags
 */
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


/**
 * lw_utf8_validate:
 * @TEXT: A string to validate as well-formed utf-8
 * @length: The length of the string or 0 to calculate it
 * @progress: (transfer none) (allow-none): A #LwProgress to track progress or %NULL
 *
 * Returns: %TRUE if @TEXT is valid utf-8
 */
gboolean
lw_utf8_validate (const gchar * TEXT,
                  gsize         length,
                  LwProgress  * progress)
{
    //Sanity checks
    if (TEXT == NULL) return FALSE;
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, FALSE);

    //Declarations
    gsize max_chunk = 0;
    gunichar c = 0;
    const gchar * p = NULL;
    const gchar * n = NULL;
    gboolean is_valid = FALSE;
    gsize offset = 0;
    GError * error = NULL;

    //Initializations
    
    if (length < 1) length = strlen(TEXT);
    max_chunk = LW_PROGRESS_START (progress, length);
    p = TEXT;
    if (p == NULL) goto errored;

    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "%s...", gettext("Validating"));
    }

    {
      gsize chunk = 0;
      gsize current = 0;
      gsize bytes_read = 0;

      while (*p != '\0' && current < length)
      {
        c = g_utf8_get_char_validated (p, length - offset);
        if (G_UNLIKELY (c == (gunichar) -1 || c == (gunichar) -2))
        {
          error = g_error_new (
            LW_UTF8_ERROR,
            LW_UTF8_ERRORCODE_VALIDATION_ERROR,
            "Invalid utf8 character at offset %d",
            offset
          );
          LW_PROGRESS_TAKE_ERROR (progress, error);
        }

        n = g_utf8_next_char (p);
        bytes_read = n - p;
        current += bytes_read;
        chunk += bytes_read;

        LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);

        p = n;
      }

      LW_PROGRESS_FINISH (progress, current);
    }

    is_valid = TRUE;

errored:

    return is_valid;
}


/**
 * lw_utf8_normalize:
 * @TEXT: A string to normalize.  A copy will be made
 * @length: Length of the string in bytes or 0 to have it calculated
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
                   gsize         length,
                   LwUtf8Flag    flags)
{
    //Sanity checks
    if (TEXT == NULL) return NULL;

    //Declarations
    gchar *buffer = NULL;
    gchar *temp = NULL;
    if (length < 1) length = -1;
    
    //Initializations
    if (flags & LW_UTF8FLAG_COMPARABLE)
    {
      buffer = g_utf8_normalize (TEXT, length, G_NORMALIZE_ALL);
    }
    else if (flags & LW_UTF8FLAG_PRINTABLE)
    {
      buffer = g_utf8_normalize (TEXT, length, G_NORMALIZE_DEFAULT);
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


static gint
lw_utf8_normalize_chunk (gchar       ** output_chunk,
                         const gchar  * TEXT,
                         LwUtf8Flag     flags,
                         gsize          max_length)
{
    //Sanity checks
    if (TEXT == NULL) return 0;

    //Declarations
    const gchar *e = TEXT;
    const gchar *p = NULL;
    gint bytes_read = 0;

    while (*e != '\0' && bytes_read < max_length)
    {
      p = e;
      e = g_utf8_next_char (e);
      bytes_read = e - TEXT;
    }
    if (p != TEXT && bytes_read > max_length) e = p;

    if (max_length == 0) bytes_read = strlen(TEXT);

    if (output_chunk != NULL) *output_chunk = lw_utf8_normalize (TEXT, bytes_read, flags);

    return e - TEXT;
}


/**
 * lw_utf8_normalize_chunked:
 * @CONTENTS: Text to be normalized
 * @content_length: The length of the text to be normalized or 0 for it to be calculated
 * @flags: Mask of #LwUtf8Flags to determine the normalization mode
 * @chunk_handler (allow-none): A method to be called on each chunk, such as a file writer
 * @chunk_handler_data: (allow-none): Data to be passed to the handler
 * @progress: (transfer none) (allow-none): An #LwProgress to track progress or %NULL
 *
 * Normalizes a long string of text in a series of chunks
 * such that it is file-writing and progress-tracking fiendly.  The chunk size
 * can be set by using lw_progress_set_prefered_chunk_size(),
 * otherwise it default to the page size of your operating system.
 * Unless your string is document sized (aka MiB in size), you
 * should probably just use lw_utf8_normalize() with its simpler interface.
 */
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
    LW_PROGRESS_RETURN_IF_SHOULD_ABORT (progress);

    if (content_length < 1) content_length = strlen(CONTENTS);

    //Declarations
    gsize bytes_read = 0;
    gsize handled_bytes = 0;
    gsize bytes_normalized = 0;
    const char *C = CONTENTS;
    gchar *normalized = NULL;
    gsize max_chunk = 0;
    GError *error = NULL;
    gboolean has_error = FALSE;
    gsize current = 0;

    max_chunk = LW_PROGRESS_START (progress, content_length);

    {
      gsize left = 0;
      gsize chunk = 0;

      while (*C != '\0' && current < content_length)
      {
        bytes_read = lw_utf8_normalize_chunk (&normalized, C, flags, max_chunk);
        bytes_normalized = strlen(normalized);
        if (normalized != NULL)
        {
          handled_bytes = chunk_handler (C, bytes_read, normalized, bytes_normalized, chunk_handler_data, &error);
          g_free (normalized); normalized = NULL;
          LW_PROGRESS_TAKE_ERROR (progress, error);
          if (bytes_normalized != handled_bytes)
          {
            error = g_error_new (
              LW_UTF8_ERROR,
              LW_UTF8_ERRORCODE_NORMALIZATION_ERROR,
              "Wasn't able to fully handle chunk of normalized text."
            );
            LW_PROGRESS_TAKE_ERROR (progress, error);
          }
        }

        chunk += bytes_read;
        current += bytes_read;
        left = content_length - current;
        C += bytes_read;

        LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);
      }
    }

    LW_PROGRESS_FINISH (progress, current);

errored:

    return;
}


static GHashTable*
_get_furiganafold_hashtable ()
{
    //Declarations
    static GHashTable * table = NULL;
    static const gchar * KATAKANA = "ァアィイゥウェエォオカガキギクグケゲコゴ"
                                    "サザシジスズセゼソゾタダチヂッツヅテデト"
                                    "ドナニヌネノハバパヒビピフブプヘベペホボ"
                                    "ポマミムメモャヤュユョヨラリルレロヮワヰ"
                                    "ヱヲンヴヵヶヽヾヿ";
    static const gchar * HIRAGANA = "ぁあぃいぅうぇえぉおかがきぎくぐけげこご"
                                    "さざしじすずせぜそぞただちぢっつづてでと"
                                    "どなにぬねのはばぱひびぴふぶぷへべぺほぼ"
                                    "ぽまみむめもゃやゅゆょよらりるれろゎわゐ"
                                    "ゑをんゔゕゖゝゞゟ";
    GHashTable * table_out = NULL;

    if (table == NULL)
    {
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
    }

    table_out = table;

errored:

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
 * @text: A string to casefold inline. The string is modified in place.
 * @length: Length of the string in bytes or 0 to have it calculated
 * @progress: (transfer none) (allow-none): An #LwProgress to track progress or %NULL
 *
 * Folds the case to lower case so that case insensitive searches can be
 * accomplished.
 */
void
lw_utf8_casefold (gchar      *text,
                  gsize       length,
                  LwProgress *progress)
{
    //Sanity checks
    if (text == NULL) return;
    LW_PROGRESS_RETURN_IF_SHOULD_ABORT (progress);

    //Declarations
    gsize max_chunk = 0;
    gint chunk = 0;
    gchar * c = NULL;
    gchar * n = NULL;
    GError * error = NULL;
    
    //Initializations
    c = text;
    if (c == NULL) goto errored;

    if (length < 1) length = strlen(text);

    max_chunk = LW_PROGRESS_START (progress, length);

    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "%s...", gettext("Folding case"));
    }

    {
      gsize current = 0;
      gsize bytes_read = 0;
      while (*c != '\0' && current < length)
      {
        n = _casefold_character (c);
        bytes_read = n - c;
        current += bytes_read;
        chunk += bytes_read;
        c = n;
        
        LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);
      }

      LW_PROGRESS_FINISH (progress, current);
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
 * @text: A string to furiganafold inline.  The string is modified in place.
 * @length: Length of the string in bytes or -1 to have it calculated
 * @progress: (transfer none) (allow-none): An #LwProgress to track progress or %NULL
 *
 * Folds katakana to hiragana characters so furigana insensitive comparisons
 * can be made.
 */
void
lw_utf8_furiganafold (gchar      * text,
                      gsize        length,
                      LwProgress * progress)
{
    //Sanity checks
    if (text == NULL) return;
    LW_PROGRESS_RETURN_IF_SHOULD_ABORT (progress);

    //Declarations
    GHashTable *conversions = NULL;
    gsize max_chunk = 0;
    gsize chunk = 0;
    gchar * c = NULL;
    gchar * n = NULL;
    GError * error = NULL;
    
    //Initializations
    c = text;
    if (c == NULL) goto errored;
    conversions = _get_furiganafold_hashtable ();
    if (conversions == NULL) goto errored;
    length = strlen(text);
    max_chunk = LW_PROGRESS_START (progress, length);

    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "%s...", gettext("Folding furigana"));
    }

    {
      gsize current = 0;
      gsize bytes_read = 0;
      while (*c != '\0' && current < length) {
        n = _furiganafold_character (c, conversions);
        bytes_read = n - c;
        current += bytes_read;
        chunk += bytes_read;

        LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);

        c = n;
      }

      LW_PROGRESS_FINISH (progress, current);
    }

errored:

    return;
}


/**
 * lw_utf8_sanitize:
 * @TEXT: (transfer none): A string to sanitize
 *
 * Sanitizes a string as utf8, replacing invalid characters with spaces.
 * 
 * Returns: (transfer full): A sanitized string that needs to be freed with g_free()
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


/**
 * lw_utf8_replace_linebreaks_with_nullcharacter:
 * @contents: The content string to modify inline
 * @content_length: The length of @contents, or 0 for it to be calculated
 * @max_line_length: (allow-none): Sets the length of the longest line encountered
 * @progress: (transfer none) (allow-none): A #LwProgress to track progress or %NULL
 *
 * This is a helper method primarily meant for #LwDictionary to parse a 
 * raw dictionary file and use the contents in place.
 *
 * Returns: The total bytes read
 */
gsize
lw_utf8_replace_linebreaks_with_nullcharacter (gchar      * contents,
                                               gsize        content_length,
                                               gsize      * max_line_length,
                                               LwProgress * progress)
{
    g_return_val_if_fail (contents != NULL, 0);
    LW_PROGRESS_RETURN_VAL_IF_SHOULD_ABORT (progress, 0);

    //Declarations    
    gint num_lines = 0;
    gsize max_chunk = 0;
    GError * error = NULL;

    if (content_length < 1) content_length = strlen(contents);
    if (max_line_length != NULL) *max_line_length = 0;

    //Initializations
    max_chunk = LW_PROGRESS_START (progress, content_length);
    if (progress != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, "Delimiting lines...");
    }

    {
      gchar *c = NULL;
      gchar *p = NULL;
      gchar *e = NULL;
      gchar *n = NULL;

      gsize current = 0;
      gsize chunk = 0;
      gsize bytes_read = 0;

      n = p = c = contents;
      e = c + content_length;

      while (*c != '\0' && c < e)
      {
        while (*c == '\n' && c < e) 
        {
          if (max_line_length != NULL)
          {
            gsize length = c - p;
            if (length > *max_line_length) *max_line_length = length;
          }
          n = lw_utf8_set_null_next_char (c);
          bytes_read = n - c;
          chunk += bytes_read;
          current += bytes_read;

          LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);

          p = c = n;
          num_lines++;
        }
        while (*c != '\0' && *c != '\n' && c < e)
        {
          n = g_utf8_next_char (c);
          bytes_read = n - c;
          current += bytes_read;
          chunk += bytes_read;

          LW_PROGRESS_UPDATE (progress, current, chunk, max_chunk, error);

          c = n;
        }
      }

      LW_PROGRESS_FINISH (progress, current);
    }

errored:

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


/**
 * lw_utf8_isescaped:
 * @TEXT: The start of the text that contains the possibly escaped character
 * @CHAR_PTR: The character to check if it's escaped. It must be within @TEXT
 *
 * Checks to see if a character is escaped by backstepping until there are no
 * backslashes or the iterator hits the beginning of the string.
 *
 * Returns: %TRUE if the character is escaped
 */
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


/**
 * lw_utf8_contains_kanji:
 * @TEXT: The text to search for kanji
 *
 * Searches a string for any characters that have the han unicode script.
 * (Also known as Kanji)
 *
 * Returns: %TRUE if any of the characters match %G_UNICODE_SCRIPT_HAN
 */
gboolean
lw_utf8_contains_kanji (gchar const * TEXT)
{
    gchar const * c = NULL;
    gunichar character = 0;
    GUnicodeScript script = G_UNICODE_SCRIPT_INVALID_CODE;
    gboolean found = FALSE;

    for (c = TEXT; *c != '\0' && !found; c = g_utf8_next_char (c))
    {
      character = g_utf8_get_char (c);
      script = g_unichar_get_script (character);
      found = (script == G_UNICODE_SCRIPT_HAN);
    }

    return found;
}


/**
 * lw_utf8_contains_furigana:
 * @TEXT: The text to search for furigana
 *
 * Searches a string for any characters that have a katakana or hiragana
 * script. 
 *
 * Returns: %TRUE if any of the characters match %G_UNICODE_SCRIPT_HIRAGANA or %G_UNICODE_SCRIPT_KATAKANA
 */
gboolean
lw_utf8_contains_furigana (gchar const * TEXT)
{
    gchar const * c = NULL;
    gunichar character = 0;
    GUnicodeScript script = G_UNICODE_SCRIPT_INVALID_CODE;
    gboolean found = FALSE;

    for (c = TEXT; *c != '\0' && !found; c = g_utf8_next_char (c))
    {
      character = g_utf8_get_char (c);
      script = g_unichar_get_script (character);
      found = (script == G_UNICODE_SCRIPT_HIRAGANA || script == G_UNICODE_SCRIPT_KATAKANA);
    }

    return found;
}


/**
 * lw_utf8_contains_romaji:
 * @TEXT: The text to search for romaji
 *
 * Searches a string for any characters that are English-looking.
 *
 * Returns: %TRUE if any of the characters match %G_UNICODE_SCRIPT_LATIN
 */
gboolean
lw_utf8_contains_romaji (gchar const * TEXT)
{
    gchar const * c = NULL;
    gunichar character = 0;
    GUnicodeScript script = G_UNICODE_SCRIPT_INVALID_CODE;
    gboolean found = FALSE;

    for (c = TEXT; *c != '\0' && !found; c = g_utf8_next_char (c))
    {
      character = g_utf8_get_char (c);
      script = g_unichar_get_script (character);
      found = (script == G_UNICODE_SCRIPT_LATIN);
    }

    return found;
}


/**
 * lw_utf8_contains_number:
 * @TEXT: The text to search for numbers
 *
 * Searches a string for any characters that are ascii numbers.
 *
 * Returns: %TRUE if any of the characters match 0-9
 */
gboolean
lw_utf8_contains_number (gchar const * TEXT)
{
    gchar const * c = NULL;
    gunichar character = 0;
    GUnicodeScript script = G_UNICODE_SCRIPT_INVALID_CODE;
    gboolean found = FALSE;

    for (c = TEXT; *c != '\0' && !found; c = g_utf8_next_char (c))
    {
      found = g_ascii_isdigit (*c);
    }

    return found;
}

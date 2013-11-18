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
//! @file utilities.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


//!
//! @brief Creates an allocated path to a file.  If the FILENAME is NULL,
//!        then it is just a path to a directory.  It must be freed with
//!        g_free
//! @param PATH Used to determine which folder path to return
//! @param FILENAME The filename to be appended to the path or NULL
//! @returns Returns a constant that should be freed with g_free
//!
gchar* 
lw_util_build_filename (const LwFolderPath PATH, const char *FILENAME) 
{
    g_assert (PATH >= 0 && PATH < TOTAL_LW_PATHS);

    //Declarations
    char *base;
    char *folder;
    char *path;
    
    base = lw_util_get_environment_data_folder ();
    if (base == NULL) base = g_build_filename (g_get_user_data_dir (), PACKAGE, NULL);
    folder = NULL;
    path = NULL;

    switch (PATH)
    {
      case LW_PATH_BASE:
        folder = g_strdup (base);
        path = g_strdup (base);
        break;
      case LW_PATH_DICTIONARY:
        folder = g_build_filename (base, "dictionaries", NULL);
        path = g_build_filename (base, "dictionaries", FILENAME, NULL);
        break;
      case LW_PATH_VOCABULARY:
        folder = g_build_filename (base, "vocabulary", NULL);
        path = g_build_filename (base, "vocabulary", FILENAME, NULL);
        break;
      case LW_PATH_PLUGIN:
        folder = g_build_filename (base, "plugins", NULL);
        path = g_build_filename (base, "plugins", FILENAME, NULL);
        break;
      case LW_PATH_CACHE:
        folder = g_build_filename (base, "cache", NULL);
        path = g_build_filename (base, "cache", FILENAME, NULL);
        break;
      case LW_PATH_INDEX:
        folder = g_build_filename (base, "index", NULL);
        path = g_build_filename (base, "index", FILENAME, NULL);
        break;
      default:
        g_assert_not_reached ();
        folder = NULL;
        path = NULL;
        break;
    }
    g_free (base);

    g_mkdir_with_parents (folder, 0755);

    g_free (folder);

    return path;
}


//!
//! @brief Gets the compression type as a string
//! @param COMPRESSION The LwCompression type to use
//! @returns A constant string that should not be freed
//!
const char* 
lw_util_get_compressionname (const LwCompression COMPRESSION)
{
    char *type;

    switch (COMPRESSION)
    {
/*
      case LW_COMPRESSION_ZIP:
        g_error ("currently unsupported compression type\n");
        return "zip";
*/
      case LW_COMPRESSION_GZIP:
        type = "gz";
        break;
      case LW_COMPRESSION_NONE:
        type = "uncompressed";
        break;
      default:
        g_assert_not_reached ();
        type = NULL;
        break;
    }

    return type;
}


//!
//! @brief Gets the encoding type as a string
//! @param ENCODING The LwEncoding type to use
//! @returns A constant string that should not be freed
//!
const char* 
lw_util_get_encodingname (const LwEncoding ENCODING)
{
    char *type;

    switch (ENCODING)
    {
      case LW_ENCODING_EUC_JP:
        type = "EUC-JP";
        break;
      case LW_ENCODING_SHIFT_JS:
        type = "Shift-JS";
        break;
      case LW_ENCODING_UTF8:
        type = "UTF-8";
        break;
      default:
        g_assert_not_reached ();
        type = NULL;
        break;
    }

    return type;
}


//!
//! @brief Convenience function for seeing if a string is hiragana
//! @param input The string to check
//! @return Returns true if it is in the range
//! @see lw_util_is_katakana_str ()
//! @see lw_util_is_kanji_str ()
//! @see lw_util_is_romaji_str ()
//!
gboolean 
lw_util_is_hiragana_str (const char *input)
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
//! @see lw_util_is_hiragana_str ()
//! @see lw_util_is_kanji_str ()
//! @see lw_util_is_romaji_str ()
//!
gboolean 
lw_util_is_katakana_str (const char *input)
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
//! @see lw_util_is_hiragana_str ()
//! @see lw_util_is_kanji_str ()
//! @see lw_util_is_romaji_str ()
//!
gboolean 
lw_util_is_furigana_str (const char *input)
{
    return (lw_util_is_katakana_str (input) || lw_util_is_hiragana_str (input));
}



//!
//! @brief Convenience function for seeing if a string *starts* with kanji
//! @param input The string to check
//! @return Returns true if the function things this is a kanji string
//! @see lw_util_is_hiragana_str ()
//! @see lw_util_is_katakana_str ()
//! @see lw_util_is_romaji_str ()
//!
gboolean 
lw_util_is_kanji_ish_str (const char *input)
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
lw_util_string_has_japanese (const gchar *INPUT)
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
//! @see lw_util_is_hiragana_str ()
//! @see lw_util_is_katakana_str ()
//! @see lw_util_is_romaji_str ()
//!
gboolean 
lw_util_is_kanji_str (const char *input)
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
//! @see lw_util_is_hiragana_str ()
//! @see lw_util_is_katakana_str ()
//! @see lw_util_is_kanji_str ()
//!
gboolean 
lw_util_is_romaji_str (const char *input)
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
lw_util_is_yojijukugo_str (const char* INPUT)
{
  return (g_utf8_strlen (INPUT, -1) == 4 && lw_util_is_kanji_str (INPUT));
}


//!
//! @brief Shifts kana characters in a specific direction
//! @param input The string to shift
//! @param shift How much to shift by
//! @see lw_util_str_shift_hira_to_kata ()
//! @see lw_util_str_shift_kata_to_hira ()
//!
void 
lw_util_shift_kana_chars_in_str_by (char *input, int shift)
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
//! @see lw_util_shift_kana_chars_in_str_by ()
//! @see lw_util_str_shift_kata_to_hira ()
//!
void 
lw_util_str_shift_hira_to_kata (char input[])
{
    lw_util_shift_kana_chars_in_str_by (input, (L'ア' - L'あ'));
}


//!
//! @brief Convenience function to shift katakana to hiragana
//!
//! @param input The string to shift
//! @see lw_util_shift_kana_chars_in_str_by ()
//! @see lw_util_str_shift_hira_to_kata ()
//!
void 
lw_util_str_shift_kata_to_hira (char input[])
{
    lw_util_shift_kana_chars_in_str_by (input, (L'あ' - L'ア'));
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
lw_util_next_hira_char_from_roma (const char *input)
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
lw_util_roma_char_to_hira (const gchar *input, gchar *output)
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
//! @see lw_util_shift_kana_chars_in_str_by ()
//! @see lw_util_str_shift_hira_to_kata ()
//!
gboolean 
lw_util_str_roma_to_hira (const gchar* input, gchar* output, gint max)
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
      kana_ptr = lw_util_roma_char_to_hira (input_ptr, kana_ptr);
      if (kana_ptr == NULL || input_ptr == NULL)
        break;
      input_ptr = lw_util_next_hira_char_from_roma (input_ptr);
      if (kana_ptr == NULL || input_ptr == NULL)
        break;

      kana_ptr = &kana_ptr[strlen(kana_ptr)];
    }

    return (input_ptr != NULL && strlen (input_ptr) == 0);
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
lw_util_sanitize_string (gchar *buffer)
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


//!
//! @brief Will change a query into a & delimited set of tokens (logical and)
//!
gchar*
lw_util_normalize_string (const gchar        *TEXT,
                          gboolean            make_case_insensitive,
                          gboolean            make_furigana_insensitive)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);

    //Declarations
    gchar *buffer;
    gchar *temp;
    
    //Initializations
    buffer = g_utf8_normalize (TEXT, -1, G_NORMALIZE_ALL);

    if (make_case_insensitive && buffer != NULL)
    {
      temp = g_utf8_casefold (buffer, -1);
      g_free (buffer); buffer = temp; temp = NULL;
    }

    if (make_furigana_insensitive && buffer != NULL)
    {
      temp = lw_util_furiganafold (buffer); 
      g_free (buffer); buffer = temp; temp = NULL;
    }

    return buffer;
}


static GHashTable*
_lw_util_get_furiganafold_hashtable ()
{
    //Declarations
    GHashTable *table;
    const gchar **ptr;
    gunichar key;
    gunichar value;
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
lw_util_furiganafold (const gchar *TEXT)
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
    table = _lw_util_get_furiganafold_hashtable ();
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
//! @brief Checks for a Japanese local messages setting
//! @returns A boolean stating whether the locale is a Japanese utf8 one
//! @return Returns true if it is a japanese local
//!
gboolean 
lw_util_is_japanese_locale ()
{
    return (setlocale(LC_ALL, NULL) != NULL &&
             (
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.UTF8")  == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.UTF-8") == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.utf8")  == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP.utf-8") == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja_JP")       == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "ja")          == 0 ||
               strcmp(setlocale(LC_ALL, NULL), "japanese")    == 0
             )
           );
}


//!
//! @brief Converts the arguments passed to the program into a query search string
//! @param argc The argc argument passed to main
//! @param argv The argv argument passed to main
//! @returns An allocated string that must be freed with g_free
//!
gchar* 
lw_util_get_query_from_args (int argc, char** argv)
{
    //Sanity check
    if (argc < 2) return NULL;

    //Declarations
    char *text;
    char *query;
    int i;
    int length;
    char *ptr;

    //Initializations
    text = NULL;
    query = NULL;
    length = 0;

    //Get the required length of the combined string
    for (i = 1; i < argc; i++) 
    {
      length += strlen (argv[i]) + 1;
    }

    //Allocate it and set up the iterator
    text = (char*) malloc(length * sizeof(char) + 1);
    if (text == NULL) return NULL;
    ptr = text;

    //Copy the argument words into the query
    for (i = 1; i < argc; i++)
    { 
      strcpy(ptr, argv[i]);
      ptr += strlen(argv[i]);

      if (i == argc - 1) break;

      strcpy(ptr, " ");
      ptr += strlen(" ");
    }

    query = g_strdup (text);
    lw_util_sanitize_string (query);

    //Cleanup
    if (text != NULL) free (text);

    return query;
}


gchar*
lw_strjoinv (gchar delimitor, gchar** array, gint array_length)
{
    g_assert (array != NULL);

    //Declarations
    gint text_length;
    gint delimitor_length;
    gchar *text, *src_ptr, *tgt_ptr;
    gint i;

    //Initializations
    text_length = 0;
    delimitor_length = sizeof (delimitor);
    i = 0;

    //Calculate the needed size
    while (i < array_length)
    {
      src_ptr = *(array + i);
      if (src_ptr != NULL) 
      {
        text_length += strlen (src_ptr);
      }
      text_length += delimitor_length;
      i++;
    }

    text = g_new (gchar, text_length);

    //Concatinate the strings
    if (text != NULL)
    {
      tgt_ptr = text;
      i = 0;

      while (i < array_length)
      {
        src_ptr = *(array + i);
        if (src_ptr != NULL)
          while (*src_ptr != '\0')
            *(tgt_ptr++) = *(src_ptr++);
        *(tgt_ptr++) = delimitor;
        i++;
      }
      *(--tgt_ptr) = '\0';
    }

    return text;
}


static gboolean
lw_util_script_changed (GUnicodeScript p, GUnicodeScript n, gboolean split_kanji_furigana)
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
lw_util_delimit_script_changes (const gchar *DELIMITOR, const gchar* TEXT, gboolean split_kanji_furigana)
{
    //Sanity check
    g_return_val_if_fail (DELIMITOR != NULL && TEXT != NULL, NULL);

    //Declarations
    gchar *buffer;
    gint count;
    const gchar *source_ptr;
    gchar *target_ptr;
    GUnicodeScript this_script, previous_script;
    gunichar c;
    gboolean script_changed;
    gint delimitor_length;

    //Initializations
    count = 0;
    delimitor_length = strlen (DELIMITOR);
    this_script = previous_script = G_UNICODE_SCRIPT_INVALID_CODE;
    for (source_ptr = TEXT; *source_ptr != '\0'; source_ptr = g_utf8_next_char (source_ptr))
    {
      c = g_utf8_get_char (source_ptr);
      this_script = g_unichar_get_script (c);
      script_changed = lw_util_script_changed (previous_script, this_script, split_kanji_furigana);

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
        script_changed = lw_util_script_changed (previous_script, this_script, split_kanji_furigana);

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
lw_util_delimit_whitespace (const gchar *DELIMITOR, const gchar* TEXT)
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
lw_util_delimit_radicals (const gchar *DELIMITOR, const gchar* TEXT)
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

gchar*
lw_util_get_environment_data_folder ()
{
    gchar **environment = g_get_environ ();
    if (environment == NULL) return NULL;
    gchar *folder = g_strdup (g_environ_getenv (environment, "LIBWAEI_DATA_FOLDER"));
    g_strfreev (environment); environment = NULL;
    return folder;
}


gboolean
lw_util_is_regex_char (gchar c)
{
    return (c == '[' || 
            c == ']' || 
            c == '(' || 
            c == ')' ||
            c == '$' ||
            c == '^' ||
            c == '|' ||
            c == '*' ||
            c == '.' ||
            c == '-' ||
            c == '?');
}


gboolean
lw_util_has_regex_char (const gchar *HAYSTACK)
{
    const gchar *ptr = HAYSTACK;
    while (*ptr != '\0')
    {
      if (lw_util_is_regex_char (*ptr)) return TRUE;
      ptr = g_utf8_next_char (ptr);
    }
    return FALSE;
}


gboolean
lw_util_is_regex_compileable (const gchar *PATTERN, GError **error)
{
    GRegex *regex = g_regex_new (PATTERN, 0, 0, error);
    if (regex != NULL)
    {
      g_regex_unref (regex); regex = NULL;
      return TRUE;
    }
    return FALSE;
}


gboolean
lw_util_is_regex_pattern (const gchar *TEXT, GError **error)
{
    g_return_val_if_fail (TEXT != NULL, FALSE);
    if (error != NULL && *error != NULL) return FALSE;

    return (lw_util_has_regex_char (TEXT) && lw_util_is_regex_compileable (TEXT, error));
}



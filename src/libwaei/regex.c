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
//!  @file regex.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


#define LW_RE_COMPILE_FLAGS (G_REGEX_CASELESS | G_REGEX_OPTIMIZE)
#define LW_RE_LOCATE_FLAGS  (0)
#define LW_RE_EXIST_FLAGS   (0)

static int _regex_expressions_reference_count = 0; //!< Internal reference count for the regexes
static GRegex *_cached_regexes[LW_RE_TOTAL + 1]; //!< Globally accessable pre-compiled regexes

//!
//! @brief Initializes often used prebuilt regex expressions
//!
void 
lw_regex_initialize ()
{
    if (_regex_expressions_reference_count > 0) return;
    _regex_expressions_reference_count++;

    memset(_cached_regexes, 0, sizeof(GRegex*) * LW_RE_TOTAL);

    //Declarations
    GError *error = NULL;
    
    //Setup the built in regexes
    _cached_regexes[LW_RE_NUMBER] = g_regex_new ("[a-zA-Z][0-9]{1,4}",  LW_RE_COMPILE_FLAGS, LW_RE_LOCATE_FLAGS, &error);
    _cached_regexes[LW_RE_STROKES] = g_regex_new ("S[0-9]{1,2}",  LW_RE_COMPILE_FLAGS, LW_RE_LOCATE_FLAGS, &error);
    _cached_regexes[LW_RE_GRADE] = g_regex_new ("G[0-9]{1,2}",  LW_RE_COMPILE_FLAGS, LW_RE_LOCATE_FLAGS, &error);
    _cached_regexes[LW_RE_FREQUENCY] = g_regex_new ("F[0-9]{1,4}",  LW_RE_COMPILE_FLAGS, LW_RE_LOCATE_FLAGS, &error);
    _cached_regexes[LW_RE_JLPT] = g_regex_new ("J[0-4]{1,1}",  LW_RE_COMPILE_FLAGS, LW_RE_LOCATE_FLAGS, &error);
    _cached_regexes[LW_RE_PARENTHESES] = g_regex_new ("\\([^\\)]*\\)", G_REGEX_OPTIMIZE, 0, &error); 
    _cached_regexes[LW_RE_KANJI_DICTIONARY_SPACERS] = g_regex_new ("[.]", G_REGEX_OPTIMIZE, 0, &error);
    _cached_regexes[LW_RE_JAPANESE] = g_regex_new ("[ァ-ーぁ-ゖー-龥]+", G_REGEX_OPTIMIZE, 0, &error);
    _cached_regexes[LW_RE_ENGLISH] = g_regex_new ("a-zA-Z0-9, '-", G_REGEX_OPTIMIZE, 0, &error);
    _cached_regexes[LW_RE_CONTIGUOUS] = g_regex_new ("[^{}()\n\t\\/「」\\]\\[.' -]+", G_REGEX_OPTIMIZE, 0, &error);
    _cached_regexes[LW_RE_SECTION] = g_regex_new ("\\b([^{}()\n\t\\/「」\\]\\[.']+)\\b", G_REGEX_OPTIMIZE, 0, &error);

    if (error != NULL)
    {
      fprintf (stderr, "regex build error: %s\n", error->message);
      g_error_free (error);
      g_assert_not_reached ();
    }
}


//!
//! @brief Frees often used prebuilt regex expressions
//!
void 
lw_regex_free ()
{
    if (_regex_expressions_reference_count < 1) return;
    _regex_expressions_reference_count--;

    //Declarations
    gint i = 0;

    //Setup the built in regexes
    while (i++ < LW_RE_TOTAL)
    {
      if (_cached_regexes[i] != NULL) g_regex_unref (_cached_regexes[i]);
    }

    memset(_cached_regexes, 0, sizeof(GRegex*) * LW_RE_TOTAL);
}


gchar*
lw_regex_remove_parenthesis (const gchar* TEXT)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);
    g_return_val_if_fail (_cached_regexes[LW_RE_PARENTHESES] != NULL, NULL);

    //Declarations
    GRegex *regex = NULL;
    gchar *output = NULL;
    const gchar *replacement = "";

    //Initializations
    regex = _cached_regexes[LW_RE_PARENTHESES];
    output = g_regex_replace (regex, TEXT, -1, 0, replacement, 0, NULL); 

errored: 

    return output;
}


gchar*
lw_regex_remove_kanji_dictionary_spacers (const gchar *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);
    g_return_val_if_fail (_cached_regexes[LW_RE_PARENTHESES] != NULL, NULL);

    //Declarations
    GRegex *regex = NULL;
    gchar *output = NULL;
    const gchar *replacement = "";

    //Initializtions
    regex = _cached_regexes[LW_RE_KANJI_DICTIONARY_SPACERS];
    output = g_regex_replace_literal (regex, TEXT, -1, 0, replacement, 0, NULL);

    return output;
}


gboolean
lw_regex_get_japanese_matches (const gchar   *HAYSTACK, 
                               GMatchInfo   **match_info)
{
    //Sanity checks
    g_return_val_if_fail (HAYSTACK != NULL, FALSE);
    g_return_val_if_fail (_cached_regexes[LW_RE_JAPANESE] != NULL, FALSE);

    //Declarations
    GRegex *regex = NULL;
    
    //Initializations
    regex = _cached_regexes[LW_RE_JAPANESE];

    return g_regex_match (regex, HAYSTACK, 0, match_info);
}


gboolean
lw_regex_get_sections (const gchar   *HAYSTACK, 
                       GMatchInfo   **match_info)
{
    //Sanity checks
    g_return_val_if_fail (HAYSTACK != NULL, FALSE);
    g_return_val_if_fail (_cached_regexes[LW_RE_SECTION] != NULL, FALSE);

    //Declarations
    GRegex *regex = NULL;

    //Initializations
    regex = _cached_regexes[LW_RE_SECTION];

    return g_regex_match (regex, HAYSTACK, 0, match_info);
}


gboolean
lw_regex_get_contiguous (const gchar   *HAYSTACK, 
                         GMatchInfo   **match_info)
{
    //Sanity checks
    g_return_val_if_fail (HAYSTACK != NULL, FALSE);
    g_return_val_if_fail (_cached_regexes[LW_RE_CONTIGUOUS] != NULL, FALSE);

    //Declarations
    GRegex *regex = NULL;

    //Initializations
    regex = _cached_regexes[LW_RE_CONTIGUOUS];

    return g_regex_match (regex, HAYSTACK, 0, match_info);
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




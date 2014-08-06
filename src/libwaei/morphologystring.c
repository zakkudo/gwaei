/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Pauli Virtanen. All Rights Reserved.

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
//! @file morphologystring.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


LwMorphologyString*
lw_morphologystring_new (const gchar *TEXT, LwNormalizationFlags flags)
{
    if (TEXT == NULL) return NULL;

    //Declarations
    LwMorphologyString *self = NULL;

    //Initializations
    self = g_new0 (LwMorphologyString, 1);

    //TODO

    self->normalized = lw_utf8_normalize (self->raw, flags);
    self->tokens = NULL;
    self->lookup = g_hash_table_new (g_str_hash, g_str_equal);

    return self;
}


void
lw_morphologystring_free (LwMorphologyString *self)
{
    if (self == NULL) return;

    if (self->raw != NULL) g_free (self->raw);
    if (self->tokens != NULL) g_list_free_full (self->tokens, g_free);
    if (self->lookup != NULL) g_hash_table_unref (self->lookup);
/*
    if (self->normalized) 
    if (self->stem)
    if (self->canonical)
    if (self->spellheck)
    if (self->explanation)
*/
    memset(self, 0, sizeof(LwMorphologyString));

    g_free (self);
}


/*

//!
//! @brief Allocates a new empty LwMorphologyString object.
//!
LwMorphologyString*
lw_string_new (gchar *word,
               gchar *normalized,
               gchar *stem,
               gchar *canonical,
               gchar *spellcheck,
               gchar *explanation,
               gint start_offset,
               gint end_offset)
{
    LwMorphologyString *self;
    self = g_new0 (LwMorphologyString, 1);

    self->word = word;
    self->normalized = normalized;
    self->stem = stem;
    self->canonical = canonical;
    self->spellcheck = spellcheck;
    self->explanation = explanation;
    self->start_offset = start_offset;
    self->end_offset = end_offset;

    if (self->word && self->stem)
    {
      gint stem_length = strlen(self->stem);
      gint word_length = strlen(self->word);
      gboolean same_ascii = (g_ascii_strcasecmp (self->word, self->stem) == 0);
      if (same_ascii && word_length == stem_length)
      {
        g_free (self->stem); self->stem = NULL;
      }
    }

    return self;
}

//!
//! @brief Frees an allocated LwMorphologyString object.
//!
void 
lw_string_free (LwMorphologyString *self)
{
    if (self->word != NULL) g_free (self->word); 
    if (self->normalized != NULL) g_free (self->normalized); 
    if (self->stem != NULL) g_free (self->stem); 
    if (self->canonical != NULL) g_free (self->canonical); 
    if (self->explanation != NULL) g_free (self->explanation); 
    if (self->spellcheck != NULL) g_free (self->spellcheck); 
    if (self->regex != NULL) g_regex_unref (self->regex);

    memset(self, 0, sizeof(LwMorphologyString));

    free(self);
}


const gchar*
lw_string_get_raw (LwMorphologyString *self)
{
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->word != NULL, NULL);

    return self->word;
}


const gchar*
lw_string_get_stem (LwMorphologyString *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    if (self->stem != NULL) return self->stem;
    else return lw_string_get_raw (self);
}


const gchar*
lw_string_get_normalized (LwMorphologyString *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    if (self->normalized != NULL) return self->normalized;
    else return lw_string_get_raw (self);
}


const gchar*
lw_string_get_canonical (LwMorphologyString *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    if (self->canonical != NULL) return self->canonical;
    else return lw_string_get_stem (self); //canonical form is based on the normalized stem form
}


gchar*
lw_string_to_string (LwMorphologyString *self)
{
    g_return_val_if_fail (self != NULL, NULL);

    GString *output = g_string_new ("LwMorphologyString { ");

    if (self->word)
      g_string_append_printf (output, "word = STRING { \"%s\" }, ", self->word);
    else
      g_string_append (output, "word = STRING { NULL }, ");
    
    if (self->stem)
      g_string_append_printf (output, "stem = STRING { \"%s\" } ", self->stem);
    else
      g_string_append (output, "stem = STRING { NULL } ");

    g_string_append (output, " }");

    return g_string_free (output, FALSE);
}


const gchar*
lw_string_get_regex_pattern (LwMorphologyString *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->word != NULL, NULL);

    //Check for cached value
    if (self->regex_pattern != NULL) return self->regex_pattern;

    //Declarations
    const gint FORMS_LENGTH = 4;
    gchar *forms[FORMS_LENGTH];
    forms[0] = self->word;
    forms[1] = self->normalized;
    forms[2] = self->stem;
    forms[3] = self->canonical;
    gint i = 0;
    GString *buffer = g_string_new("");

    for (i = 0; i < FORMS_LENGTH; i++)
    {
      if (forms[i] == NULL) continue;

      if (*(buffer->str) == '\0') g_string_assign (buffer, forms[i]);
      else g_string_append_printf (buffer, "|%s", forms[i]);
    }

    if (self->regex_pattern != NULL) g_free (self->regex_pattern);
    self->regex_pattern = g_string_free (buffer, FALSE); buffer = NULL;

errored:

    if (buffer != NULL) g_string_free (buffer, TRUE); buffer = NULL;

    return self->regex_pattern;
}


GRegex*
lw_string_get_regex (LwMorphologyString *self, GError **error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    if (self->regex != NULL) return self->regex;

    //Declarations
    const gchar *pattern = NULL;
    GRegex *regex = NULL;

    //Initializtions
    pattern = lw_string_get_regex_pattern (self); if (pattern == NULL) goto errored;
    regex = g_regex_new (pattern, G_REGEX_OPTIMIZE, 0, error); if (regex == NULL) goto errored;

    if (self->regex != NULL) g_regex_unref (self->regex);
    self->regex = regex; regex = NULL;

errored:

    if (regex != NULL) g_regex_unref (regex); regex = NULL;

    return self->regex;
}


gboolean
lw_string_regex_search (LwMorphologyString *self, 
                        const gchar  *HAYSTACK,
                        GMatchInfo   **match_info)
{
    //Sanity checsk
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (HAYSTACK != NULL, FALSE);

    //Declarations
    GRegex *regex = NULL;

    regex = lw_string_get_regex (self, NULL); 

    return g_regex_match (regex, HAYSTACK, 0, match_info);
}


///!
///! @brief Returns a GList of gint32 variables stored in the pointer.  Free using g_list_free().
///!
GList*
lw_string_get_morphologyindexlist (LwMorphologyString *self, 
                                   const gchar  *HAYSTACK,
                                   gint          index)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (HAYSTACK != NULL, NULL);

    //Declarations
    GList *offsetlist = NULL;
    GMatchInfo *match_info = NULL;
    gint total = 0;
    GRegex *regex = NULL;
    
    //Initializations
    regex = lw_string_get_regex (self, NULL); if (regex == NULL) goto errored;
    if (g_regex_match (regex, HAYSTACK, 0, &match_info) == FALSE) goto errored;
    total = g_match_info_get_match_count (match_info); if (total < 1) goto errored;

    while (g_match_info_matches (match_info) && total--)
    { 
      gint start_offset = 0;
      gint end_offset = 0;
      LwMorphologyIndex *morphologyindex = NULL;

      if (g_match_info_fetch_pos (match_info, 0, &start_offset, &end_offset))
      {
        morphologyindex = lw_morphologyindex_new (index, HAYSTACK, start_offset, end_offset);
        offsetlist = g_list_append (offsetlist, morphologyindex);
      }

      g_match_info_next (match_info, NULL);
    }
    
errored:

    if (match_info != NULL) g_match_info_free (match_info); match_info = NULL;

    return offsetlist;
}

*/

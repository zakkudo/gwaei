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
//! @file morphology.c
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


//!
//! @brief Allocates a new empty LwMorphology object.
//!
LwMorphology*
lw_morphology_new (gchar *word,
                   gchar *normalized,
                   gchar *stem,
                   gchar *canonical,
                   gchar *spellcheck,
                   gchar *explanation,
                   gint start_offset,
                   gint end_offset)
{
    LwMorphology *morphology;
    morphology = g_new0 (LwMorphology, 1);

    morphology->word = word;
    morphology->normalized = normalized;
    morphology->stem = stem;
    morphology->canonical = canonical;
    morphology->spellcheck = spellcheck;
    morphology->explanation = explanation;
    morphology->start_offset = start_offset;
    morphology->end_offset = end_offset;

    if (morphology->word && morphology->stem)
    {
      gint stem_length = strlen(morphology->stem);
      gint word_length = strlen(morphology->word);
      gboolean same_ascii = (g_ascii_strcasecmp (morphology->word, morphology->stem) == 0);
      if (same_ascii && word_length == stem_length)
      {
        g_free (morphology->stem); morphology->stem = NULL;
      }
    }

    return morphology;
}

//!
//! @brief Frees an allocated LwMorphology object.
//!
void 
lw_morphology_free (LwMorphology *morphology)
{
    if (morphology->word != NULL) g_free (morphology->word); 
    if (morphology->normalized != NULL) g_free (morphology->normalized); 
    if (morphology->stem != NULL) g_free (morphology->stem); 
    if (morphology->canonical != NULL) g_free (morphology->canonical); 
    if (morphology->explanation != NULL) g_free (morphology->explanation); 
    if (morphology->spellcheck != NULL) g_free (morphology->spellcheck); 
    if (morphology->regex != NULL) g_regex_unref (morphology->regex);

    memset(morphology, 0, sizeof(LwMorphology));

    free(morphology);
}


const gchar*
lw_morphology_get_raw (LwMorphology *morphology)
{
    g_return_val_if_fail (morphology != NULL, NULL);
    g_return_val_if_fail (morphology->word != NULL, NULL);

    return morphology->word;
}


const gchar*
lw_morphology_get_stem (LwMorphology *morphology)
{
    g_return_val_if_fail (morphology != NULL, NULL);

    if (morphology->stem != NULL) return morphology->stem;
    else return lw_morphology_get_raw (morphology);
}


const gchar*
lw_morphology_get_normalized (LwMorphology *morphology)
{
    g_return_val_if_fail (morphology != NULL, NULL);

    if (morphology->normalized != NULL) return morphology->normalized;
    else return lw_morphology_get_raw (morphology);
}


const gchar*
lw_morphology_get_canonical (LwMorphology *morphology)
{
    g_return_val_if_fail (morphology != NULL, NULL);

    if (morphology->canonical != NULL) return morphology->canonical;
    else return lw_morphology_get_stem (morphology); //canonical form is based on the normalized stem form
}


gchar*
lw_morphology_to_string (LwMorphology *morphology)
{
    g_return_val_if_fail (morphology != NULL, NULL);

    GString *output = g_string_new ("LwMorphology { ");

    if (morphology->word)
      g_string_append_printf (output, "word = STRING { \"%s\" }, ", morphology->word);
    else
      g_string_append (output, "word = STRING { NULL }, ");
    
    if (morphology->stem)
      g_string_append_printf (output, "stem = STRING { \"%s\" } ", morphology->stem);
    else
      g_string_append (output, "stem = STRING { NULL } ");

    g_string_append (output, " }");

    return g_string_free (output, FALSE);
}


const gchar*
lw_morphology_get_regex_pattern (LwMorphology *morphology)
{
    //Sanity checks
    g_return_val_if_fail (morphology != NULL, NULL);
    g_return_val_if_fail (morphology->word != NULL, NULL);

    //Check for cached value
    if (morphology->regex_pattern != NULL) return morphology->regex_pattern;

    //Declarations
    const gint FORMS_LENGTH = 4;
    gchar *forms[FORMS_LENGTH];
    forms[0] = morphology->word;
    forms[1] = morphology->normalized;
    forms[2] = morphology->stem;
    forms[3] = morphology->canonical;
    gint i = 0;
    GString *buffer = g_string_new("");

    for (i = 0; i < FORMS_LENGTH; i++)
    {
      if (forms[i] == NULL) continue;

      if (*(buffer->str) == '\0') g_string_assign (buffer, forms[i]);
      else g_string_append_printf (buffer, "|%s", forms[i]);
    }

    if (morphology->regex_pattern != NULL) g_free (morphology->regex_pattern);
    morphology->regex_pattern = g_string_free (buffer, FALSE); buffer = NULL;

errored:

    if (buffer != NULL) g_string_free (buffer, TRUE); buffer = NULL;

    return morphology->regex_pattern;
}


GRegex*
lw_morphology_get_regex (LwMorphology *morphology, GError **error)
{
    //Sanity checks
    g_return_val_if_fail (morphology != NULL, NULL);

    if (morphology->regex != NULL) return morphology->regex;

    //Declarations
    const gchar *pattern = NULL;
    GRegex *regex = NULL;

    //Initializtions
    pattern = lw_morphology_get_regex_pattern (morphology); if (pattern == NULL) goto errored;
    regex = g_regex_new (pattern, G_REGEX_OPTIMIZE, 0, error); if (regex == NULL) goto errored;

    if (morphology->regex != NULL) g_regex_unref (morphology->regex);
    morphology->regex = regex; regex = NULL;

errored:

    if (regex != NULL) g_regex_unref (regex); regex = NULL;

    return morphology->regex;
}


gboolean
lw_morphology_regex_search (LwMorphology *morphology, 
                            const gchar  *HAYSTACK,
                            GMatchInfo   **match_info)
{
    //Sanity checsk
    g_return_val_if_fail (morphology != NULL, FALSE);
    g_return_val_if_fail (HAYSTACK != NULL, FALSE);

    //Declarations
    GRegex *regex = NULL;

    regex = lw_morphology_get_regex (morphology, NULL); 

    return g_regex_match (regex, HAYSTACK, 0, match_info);
}


///!
///! @brief Returns a GList of gint32 variables stored in the pointer.  Free using g_list_free().
///!
GList*
lw_morphology_get_morphologyindexlist (LwMorphology *morphology, 
                                        const gchar  *HAYSTACK,
                                        gint          index)
{
    //Sanity checks
    g_return_val_if_fail (morphology != NULL, NULL);
    g_return_val_if_fail (HAYSTACK != NULL, NULL);

    //Declarations
    GList *offsetlist = NULL;
    GMatchInfo *match_info = NULL;
    gint total = 0;
    GRegex *regex = NULL;
    
    //Initializations
    regex = lw_morphology_get_regex (morphology, NULL); if (regex == NULL) goto errored;
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



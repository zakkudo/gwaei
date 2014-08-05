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
//! @file morphology-mecab.c
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

//
// Morphological analysis using the Mecab engine
//

// Keywords used by Mecab dictionaries (ID numbers vary between different
// dictionaries, and cannot be relied on, so just compare UTF-8 strings)
#define ID_VERB          "動詞"
#define ID_NOUN          "名詞"
#define ID_SUFFIX        "接尾辞"
#define ID_POSTPOSITION  "助詞"
#define ID_AUX_VERB      "助動詞"
#define ID_NON_INDEPENDENT   "非自立"

#define PLAIN_COPULA "だ"


//!
//! @brief Convert string from UTF-8 to Mecab's charset.
//!
static gchar*
_lw_morphologyengine_mecab_encode (LwMorphologyEngine *engine, 
                                  const gchar        *WORD, 
                                  gint                nbytes)
{
    const mecab_dictionary_info_t *info = mecab_dictionary_info (engine->mecab);
    gsize bytes_read, bytes_written;
    return g_convert (WORD, nbytes, info->charset, "UTF-8", &bytes_read, &bytes_written, NULL);
}

//!
//! @brief Convert string from Mecab's charset to UTF-8.
//!
static gchar*
_lw_morphologyengine_mecab_decode (LwMorphologyEngine *engine, 
                                  const gchar        *WORD,
                                  gint                nbytes)
{
    const mecab_dictionary_info_t *info = mecab_dictionary_info (engine->mecab);
    gsize bytes_read, bytes_written;
    return g_convert (WORD, nbytes, "UTF-8", info->charset, &bytes_read, &bytes_written, NULL);
}




gboolean
_is_primary_part_of_speech (gchar **fields)
{
    return (strcmp("助詞", fields[0]) != 0 && strcmp("助動詞", fields[0]) != 0 && strcmp("接尾", fields[1]) != 0);
}


static GList*
_lw_morphologyengine_append_morphology (GList *list, gchar *WORD, gchar *STEM, gint start_offset, gint end_offset)
{
    if (WORD == NULL) return list;

    //printf("BREAK primary: %s %s %d %d\n", WORD, STEM, start_offset, end_offset);

    gchar *normalized = NULL;
    gchar *canonical = NULL;

    if (WORD != NULL) normalized = lw_utf8_normalize (WORD, FALSE, TRUE);
    if (STEM != NULL) canonical =  lw_utf8_normalize (STEM, FALSE, TRUE);

    if (normalized != NULL && strcmp(WORD, normalized) == 0) { g_free (normalized); normalized = NULL; }
    if (STEM != NULL && canonical != NULL && strcmp(STEM, canonical) == 0) { g_free (canonical); canonical = NULL; }

    LwMorphology* morphology = lw_morphology_new (
      WORD, 
      normalized, 
      STEM, 
      canonical, 
      NULL,  
      NULL, 
      start_offset, 
      end_offset
    );

    return g_list_append (list, morphology);
}


GList*
lw_morphologyengine_mecab_kanji_ish_analyze (LwMorphologyEngine *engine, 
                                             const gchar        *INPUT_RAW)
{
    //Sanity checks
    if (engine == NULL) return NULL;
    if (engine->mecab == NULL) return NULL;
    g_return_val_if_fail (INPUT_RAW != NULL, NULL);

    //Declarations
    const mecab_node_t *node = NULL;
    gchar *query = NULL;
    gchar *result = NULL;
    gchar **fields = NULL;
    gchar *surface = NULL;

    gchar *word = NULL;
    gchar *stem = NULL;

    gint start_offset = 0, end_offset = 0;

    GList *list = NULL;

    //printf("BREAK query: %s\n", INPUT_RAW);

    //Initializations
    query = _lw_morphologyengine_mecab_encode (engine, INPUT_RAW, -1);
    
    //Analysis
    for (node = mecab_sparse_tonode (engine->mecab, query); node != NULL; node = node->next)
    {
      if (node->stat != MECAB_NOR_NODE) continue;

      result = _lw_morphologyengine_mecab_decode (engine, node->feature, -1); if (result == NULL) goto errored;
      surface = _lw_morphologyengine_mecab_decode (engine, node->surface, -1); if (result == NULL) goto errored;
      fields = g_strsplit (result, ",", -1); if (fields == NULL) goto errored;
      end_offset = strlen(INPUT_RAW) - strlen(surface);
  //    printf("BREAK surface: %s %d %d\n", surface, start_offset, end_offset);

      if (g_strv_length (fields) >= 7 && _is_primary_part_of_speech (fields))
      {
        if (end_offset > start_offset)
        {
   //         printf("BREAK secondary %d %d\n", start_offset, end_offset);
            gint bytes = end_offset - start_offset;
            if (word != NULL) g_free (word); word = g_strndup (INPUT_RAW + start_offset, bytes);
        }

        if (word != NULL)
        {
    //      printf("BREAK primary: %s %s %d %d\n", word, stem, start_offset, end_offset);
          list = _lw_morphologyengine_append_morphology (list, word, stem, start_offset, end_offset);
          word = stem = NULL;
        }

        if (stem != NULL) g_free (stem); stem = g_strdup (fields[6]);

        start_offset = end_offset;
      }

errored:

      if (result != NULL) g_free (result); result = NULL;
      if (surface != NULL) g_free (surface); surface = NULL;
      if (fields != NULL) g_strfreev (fields); fields = NULL;
    }

    //Grab the final morphology
    end_offset = strlen(INPUT_RAW);

    if (end_offset > start_offset)
    {
        //printf("BREAK secondary %d %d\n", start_offset, end_offset);
        gint bytes = end_offset - start_offset;
        if (word != NULL) g_free (word); word = g_strndup (INPUT_RAW + start_offset, bytes);
    }

    if (word != NULL)
    {
      //printf("BREAK primary: %s %s %d %d\n", word, stem, start_offset, end_offset);
      list = _lw_morphologyengine_append_morphology (list, word, stem, start_offset, end_offset);
      word = stem = NULL;
    }

    return list;
}


mecab_t*
lw_morphologyengine_mecab_new ()
{
    mecab_t *mecab = mecab_new2(""); if (mecab == NULL) goto errored;

errored:

    return mecab;
}


static gboolean 
_has_kanji (const gchar* TEXT)
{
    //Sanity checks
    if (TEXT == NULL) return FALSE;

    //Declarations
    const gchar *ptr = TEXT;
    gunichar c = 0;
    GUnicodeScript script = G_UNICODE_SCRIPT_INVALID_CODE;
    gboolean has_kanji = FALSE;

    while (*ptr != '\0' && !has_kanji)
    {
      c = g_utf8_get_char (ptr);
      script = g_unichar_get_script (c);
      if (script == G_UNICODE_SCRIPT_HAN && strcmp("ー", ptr) != 0) has_kanji = TRUE;

      ptr = g_utf8_next_char (ptr);
    }

    return has_kanji;
}


GList*
lw_morphologyengine_mecab_analyze (LwMorphologyEngine *engine,
                                   const gchar        *HAYSTACK)
{
    //Sanity checks
    g_return_val_if_fail (engine != NULL, NULL);
    if (engine->mecab == NULL) return NULL;
    g_return_val_if_fail (HAYSTACK != NULL, NULL);

    //Declarations
    GList *list = NULL;
    gint start_offset = 0, end_offset = 0;
    GMatchInfo *match_info = NULL;
    gchar *word = NULL;

    //Body
    lw_regex_get_japanese_matches (HAYSTACK, &match_info);

    while (g_match_info_matches (match_info))
    {
        word = g_match_info_fetch (match_info, 0);
        if (word != NULL)
        {
          g_match_info_fetch_pos (match_info, 0, &start_offset, &end_offset);

          //Generate the forms
          if (_has_kanji (word))
            list = g_list_concat (list, lw_morphologyengine_mecab_kanji_ish_analyze (engine, word)); //mecab is horrible with sentences without kanji
          else
            list = _lw_morphologyengine_append_morphology (list, word, NULL, 0, 0);

          word = NULL; //These functions steal word
        }
        else if (word != NULL)
        {
          g_free (word); word = NULL;
        }
        g_match_info_next (match_info, NULL);
    }

errored:

    if (match_info != NULL) g_match_info_free (match_info); match_info = NULL;

    return list;
}

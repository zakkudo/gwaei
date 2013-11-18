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
//! @file morphologylist.c
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



LwMorphologyList*
lw_morphologylist_new_from_list (GList *list)
{
    LwMorphologyList *temp = g_new0 (LwMorphologyList, 1);
    if (temp == NULL) goto errored;
  
    temp->list = temp->link = list;

errored:

    return temp;
}


void
lw_morphologylist_rewind (LwMorphologyList *morphologylist)
{
    //Sanity checks
    g_return_if_fail (morphologylist != NULL);

    morphologylist->link = morphologylist->list;
}


LwMorphology*
lw_morphologylist_read (LwMorphologyList *morphologylist)
{
    //Sanity checks
    g_return_val_if_fail (morphologylist != NULL, NULL);

    LwMorphology *morphology = NULL;

    if (morphologylist->link != NULL)
    {
      morphology = LW_MORPHOLOGY (morphologylist->link->data);
      morphologylist->link = morphologylist->link->next;
    }

    return morphology;
}


//!
//! @brief Convenience function to free a GList of LwMorphology
//! @param morphology The object to free
//!
void 
lw_morphologylist_free (LwMorphologyList *morphologylist)
{
    if (morphologylist == NULL) return;
    if (morphologylist->list != NULL) g_list_free_full (morphologylist->list, (GDestroyNotify)lw_morphology_free);
    memset(morphologylist, 0, sizeof(LwMorphologyList));
    g_free (morphologylist);
}


LwMorphology*
lw_morphologylist_find (LwMorphologyList *morphologylist,
                        const gchar      *WORD)
{
    GList *iter;
    LwMorphology *morphology = NULL;

    for (iter = morphologylist->list; iter != NULL; iter = iter->next)
    {
      morphology = LW_MORPHOLOGY (iter->data);
      if ((morphology->word && g_strcmp0(morphology->word, WORD) == 0) ||
          (morphology->stem && g_strcmp0 (morphology->stem, WORD) == 0))
      {
        return morphology;
      }
    }

    return NULL;
}


//!
//! @brief Returns a string representation of a LwMorphologyList object
//!
gchar*
lw_morphologylist_to_string (LwMorphologyList *morphologylist)
{
    g_return_val_if_fail (morphologylist != NULL, NULL);

    GString *output = g_string_new ("LwMorphologyList {\n");
    LwMorphology *morphology;
    GList *position = morphologylist->link;

    lw_morphologylist_rewind (morphologylist);

    while ((morphology = lw_morphologylist_read (morphologylist)) != NULL)
    {
      gchar *morphology_string = lw_morphology_to_string (morphology);
      if (morphology_string != NULL)
      {
        g_string_append_printf (output, "\t%s\n", morphology_string);
        g_free (morphology_string);
      }
    }

    g_string_append (output, " }");

    morphologylist->link = position;

    return g_string_free (output, FALSE);
}


//!
//! @brief Returns an array of string corresponding to lw_morphology_get_word().  Free with g_strfreev()
//!
gchar const**
lw_morphologylist_get_words (LwMorphologyList *morphologylist)
{
    g_return_val_if_fail (morphologylist != NULL, NULL);

    gint length = g_list_length (morphologylist->list);
    if (length == 0) return NULL;
    gchar const ** words = g_new0 (const gchar*, length + 1); if (words == NULL) return NULL;
    LwMorphology *morphology = NULL;
    GList *position = morphologylist->link;
    gint i = 0;

    lw_morphologylist_rewind (morphologylist);

    while ((morphology = lw_morphologylist_read (morphologylist)) != NULL && i < length)
    {
      words[i++] = lw_morphology_get_raw (morphology);
    }
    words[i] = NULL;

    morphologylist->link = position;

    if (i == 0)
    {
      g_free (words); words = NULL;
    }

    return words;
}


//!
//! @brief Returns an array of string corresponding to lw_morphology_get_stem().  Free with g_strfreev()
//!
gchar const**
lw_morphologylist_get_stems (LwMorphologyList *morphologylist)
{
    g_return_val_if_fail (morphologylist != NULL, NULL);

    gint length = g_list_length (morphologylist->list);
    if (length == 0) return NULL;
    gchar const ** stems = g_new0 (const gchar*, length + 1); if (stems == NULL) return NULL;
    LwMorphology *morphology = NULL;
    GList *position = morphologylist->link;
    int i = 0;

    lw_morphologylist_rewind (morphologylist);

    while ((morphology = lw_morphologylist_read (morphologylist)) != NULL && i < length)
    {
      stems[i++] = lw_morphology_get_stem (morphology);
    }
    stems[i] = NULL;

    morphologylist->link = position;

    if (i == 0)
    {
      g_free (stems); stems = NULL;
    }

    return stems;
}


//!
//! @brief Returns an array of string corresponding to lw_morphology_get_normalized().  Free with g_strfreev()
//!
gchar const**
lw_morphologylist_get_normalized (LwMorphologyList *morphologylist)
{
    g_return_val_if_fail (morphologylist != NULL, NULL);

    gint length = g_list_length (morphologylist->list);
    gchar const ** normalized = g_new0 (const gchar*, length + 1); if (normalized == NULL) return NULL;
    LwMorphology *morphology = NULL;
    GList *position = morphologylist->link;
    int i = 0;

    lw_morphologylist_rewind (morphologylist);

    while ((morphology = lw_morphologylist_read (morphologylist)) != NULL && i < length)
    {
      normalized[i++] = lw_morphology_get_normalized (morphology);
    }
    normalized[i] = NULL;

    morphologylist->link = position;

    if (i == 0)
    {
      g_free (normalized); normalized = NULL;
    }

    return normalized;
}


//!
//! @brief Returns an array of string corresponding to lw_morphology_get_canonical().  Free with g_strfreev()
//!
gchar const**
lw_morphologylist_get_canonical (LwMorphologyList *morphologylist)
{
    g_return_val_if_fail (morphologylist != NULL, NULL);

    gint length = g_list_length (morphologylist->list);
    if (length == 0) return NULL;
    gchar const ** canonical = g_new0 (const gchar*, length + 1); if (canonical == NULL) return NULL;
    LwMorphology *morphology = NULL;
    GList *position = morphologylist->link;
    int i = 0;

    lw_morphologylist_rewind (morphologylist);

    while ((morphology = lw_morphologylist_read (morphologylist)) != NULL && i < length)
    {
      canonical[i++] = lw_morphology_get_canonical (morphology);
    }
    canonical[i] = NULL;

    morphologylist->link = position;

    if (i == 0)
    {
      g_free (canonical); canonical = NULL;
    }

    return canonical;
}


gint 
lw_morphologylist_length (LwMorphologyList *morphologylist)
{
    g_return_val_if_fail (morphologylist != NULL, 0);

    if (morphologylist->list == NULL) return 0;
    return (g_list_length (morphologylist->list));
}


static gint
_sort (gconstpointer a, gconstpointer b)
{
    const LwMorphologyIndex *ia = a;
    const LwMorphologyIndex *ib = b;

    if (ia->start_offset < ib->start_offset) return -1;
    if (ia->start_offset > ib->start_offset) return 1;
    if (ia->start_offset == ib->start_offset) 
    {
      if (ia->end_offset < ib->end_offset) return -1;
      if (ia->end_offset > ib->end_offset) return 1;
      if (ia->end_offset == ib->end_offset) return 0;
    }

    g_assert_not_reached ();
    return 0;
}


static gint
_lw_morphologylist_get_section_score (LwMorphologyList  *morphologylist, 
                                      const gchar       *HAYSTACK)
{
    //Sanity checks
    g_return_val_if_fail (morphologylist != NULL, NULL);
    g_return_val_if_fail (HAYSTACK != NULL, NULL);
    if (strlen(HAYSTACK) == 0) return G_MININT;

    //Declarations
    GList *link = morphologylist->list;
    GList *resultlist = NULL;
    gint index = 0;
    gint score = 0;
    const gint DIFFERENCE_SCORE_WEIGHT = 1;
    const gint CONTIGUOUS_SCORE_WEIGHT = 10;
    const gint IN_ORDER_SCORE_WEIGHT = 5;
    const gint STARTS_WITH_SCORE_WEIGHT = 5;

    //Calculate the LwMorphologyIndex
    for (link = morphologylist->list; link != NULL; link = link->next)
    {
      LwMorphology *morphology = LW_MORPHOLOGY (link->data);
      GMatchInfo *match_info = NULL;

      if (lw_morphology_regex_search (morphology, HAYSTACK, &match_info))
      {
        while (g_match_info_matches (match_info))
        {
          gint start_offset = 0;
          gint end_offset = 0;
          if (g_match_info_fetch_pos (match_info, 0, &start_offset, &end_offset))
          {
            LwMorphologyIndex *morphologyindex = lw_morphologyindex_new (index, HAYSTACK, start_offset, end_offset);
            resultlist = g_list_append (resultlist, morphologyindex);
            index++;
          }

          g_match_info_next (match_info, NULL);
        }
       
        if (match_info != NULL) g_match_info_free (match_info); match_info = NULL;
      }
    }
    resultlist = g_list_sort (resultlist, _sort);

    if (resultlist == NULL) return G_MININT;


    gint starts_with_score = 0;
    {
      //Get the ratio of correct charaters to incorrect
      link = resultlist;
      if (link != NULL)
      {
        LwMorphologyIndex *morphologyindex = link->data;
        if (morphologyindex->start_offset == 0)
        {
          starts_with_score += 10;
        }
      }
    }
    score += starts_with_score * STARTS_WITH_SCORE_WEIGHT;


    gint difference_score = 0;
    {
      //Get the ratio of correct charaters to incorrect
      difference_score = -g_utf8_strlen (HAYSTACK, -1);
      for (link = resultlist; link != NULL; link = link->next)
      {
        LwMorphologyIndex *morphologyindex = link->data;
        const gchar *SUB_INDEX = HAYSTACK + morphologyindex->start_offset;
        gint length = morphologyindex->end_offset - morphologyindex->start_offset;
        difference_score += g_utf8_strlen (SUB_INDEX, length);
      }
    }
    score += difference_score * DIFFERENCE_SCORE_WEIGHT;


    gint contiguous_score = 0;
    {
      //Figure out of the tokens are in order and/or contiguous
      GHashTable *table = g_hash_table_new (g_direct_hash, g_direct_equal);

      //Generate the index
      for (link = resultlist; link != NULL; link = link->next)
      {
        LwMorphologyIndex *morphologyindex = link->data;

        g_hash_table_insert (table, 
                             GINT_TO_POINTER (morphologyindex->start_offset), 
                             morphologyindex);
        g_hash_table_insert (table, 
                             GINT_TO_POINTER (morphologyindex->end_offset), 
                             morphologyindex);
      }

      //Calculate the contiguous score
      for (link = resultlist; link != NULL; link = link->next)
      {
        LwMorphologyIndex *primaryindex = link->data;
        gint index = primaryindex->index;
        gint offset = primaryindex->end_offset;

        while (*(HAYSTACK + offset) == ' ' || *(HAYSTACK + offset) == '-') offset++;
        LwMorphologyIndex *otherindex = g_hash_table_lookup (table, GINT_TO_POINTER (offset));
        
        if (otherindex != NULL && otherindex->index == index + 1) contiguous_score++;
      }

      g_hash_table_unref (table); table = NULL;
    }
    score += contiguous_score * CONTIGUOUS_SCORE_WEIGHT;


    gint in_order_score = 0;
    {
      for (link = resultlist; link != NULL && link->next != NULL; link = link->next)
      {
        LwMorphologyIndex *primaryindex = link->data;
        LwMorphologyIndex *otherindex = link->next->data;
        if (primaryindex->index == otherindex->index) ;
        else if (primaryindex->index + 1 == otherindex->index) in_order_score++;
        else in_order_score--;
      }
    }
    score += in_order_score * IN_ORDER_SCORE_WEIGHT;
    //fprintf(stderr, "BREAK %s %d\n", HAYSTACK, difference);

errored:

    if (resultlist != NULL) g_list_free_full (resultlist, (GDestroyNotify) lw_morphologyindex_free); resultlist = NULL;

    return score;
}


///!
///! @brief returns an unsorted morphologyindexlist, used for findings postions of matches and their order
///!
gint
lw_morphologylist_get_score (LwMorphologyList *morphologylist, 
                             const gchar      *HAYSTACK)
{
    //Sanity checks
    g_return_val_if_fail (morphologylist != NULL, NULL);
    g_return_val_if_fail (HAYSTACK != NULL, NULL);

    //Declarations
    gint i = 0;
    gint score = G_MININT;
    gchar *cleaned = NULL;
    GMatchInfo *match_info = NULL;

    //Initializations
    cleaned = lw_regex_remove_parenthesis (HAYSTACK); if (cleaned == NULL) goto errored;
    lw_regex_get_sections (cleaned, &match_info); 

    while (g_match_info_matches (match_info))
    {
      gchar *mini_haystack = g_match_info_fetch (match_info, 0);
      gint section_score = _lw_morphologylist_get_section_score (morphologylist, mini_haystack);
//      printf("BREAK mini %d %s\n", section_score, mini_haystack);

      if (section_score > score) score = section_score;

      if (mini_haystack != NULL) g_free (mini_haystack); mini_haystack = NULL;

      g_match_info_next (match_info, NULL);
    }
//    printf("BREAK %d %s\n", score, HAYSTACK);
     
errored:

    if (match_info != NULL) g_match_info_free (match_info); match_info = NULL;
    if (cleaned != NULL) g_free (cleaned); cleaned = NULL;

    return score;
}


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
//! @file searchiterator.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


LwSearchIterator*
lw_searchiterator_new (LwSearch    *search, 
                             const gchar *CATEGORY)
{
    //Sanity checks
    g_return_val_if_fail (search != NULL, NULL);
    g_return_val_if_fail (CATEGORY != NULL, NULL);

    //Declarations
    LwSearchIterator *iterator = NULL;

    //Initializations
    iterator = g_new0 (LwSearchIterator, 1); if (iterator == NULL) goto errored;
    iterator->search = search;
    iterator->category = g_strdup (CATEGORY);
    iterator->count = 0;
    iterator->historytable = g_hash_table_new (g_direct_hash, g_direct_equal);

    return iterator;

errored:

    if (iterator != NULL) lw_searchiterator_free (iterator);

    lw_search_unlock (search);

    return NULL;
}


LwResult*
lw_searchiterator_get_result (LwSearchIterator* iterator)
{
    //Sanity checks
    g_return_val_if_fail (iterator != NULL, NULL);
    if (iterator->link == NULL) return NULL;

    //Declarations
    LwResult *result = NULL;
    LwOffset offset = GPOINTER_TO_OFFSET (iterator->link->data);
    LwSearch *search = iterator->search;
    LwDictionary *dictionary = search->dictionary;
    const gchar *TEXT = lw_dictionary_get_string (dictionary, offset); if (TEXT == NULL) goto errored;

    //Initializtions
    result = lw_result_new (); if (result == NULL) goto errored;

    lw_dictionary_parse_result (dictionary, result, TEXT);

errored:

    return result;
}


gboolean
lw_searchiterator_next (LwSearchIterator* iterator)
{
    gboolean can_have_more_results = FALSE;
    LwSearch *search = iterator->search;
    gboolean moved = FALSE;

    lw_search_lock (search);

    LwSearchStatus status = search->status;
    gboolean exists = FALSE;

    do {
      //Initialize the iterator
      if (iterator->list == NULL && status != LW_SEARCHSTATUS_IDLE)
      {
        iterator->list = g_hash_table_lookup (search->resulttable, iterator->category); 
        iterator->link = iterator->list;
        if (iterator->link != NULL)
        {
          iterator->count++;
          moved = TRUE;
        }
        else
        {
          if (status == LW_SEARCHSTATUS_FINISHING) search->status = LW_SEARCHSTATUS_IDLE;
        }
      }
      //Get the next link if it exists
      else if (iterator->link != NULL && iterator->link->next != NULL && status != LW_SEARCHSTATUS_IDLE)
      {
        iterator->link = iterator->link->next;
        moved = TRUE;
        iterator->count++;
      }
      //Cleanup if we are finished
      else if (status == LW_SEARCHSTATUS_FINISHING || LW_SEARCHSTATUS_CANCELING)
      {
        search->status = LW_SEARCHSTATUS_IDLE;
      }

      //Record the offset so we have no repeats
      if (iterator->link != NULL && iterator->link->data != NULL)
      {
        LwOffset offset = GPOINTER_TO_OFFSET (iterator->link->data);
        gpointer key = LW_OFFSET_TO_POINTER (offset);
        exists = g_hash_table_contains (iterator->historytable, key);

        if (!exists) g_hash_table_add (iterator->historytable, key);
      }

    } while (exists && iterator->link != NULL && iterator->link->next != NULL);

    lw_search_unlock (search);
     
    return moved;
}


void
lw_searchiterator_rewind (LwSearchIterator* iterator)
{
    //Sanity checks
    g_return_if_fail (iterator != NULL);

    iterator->link = iterator->list;
    iterator->count = 0;
    g_hash_table_remove_all (iterator->historytable);
}


void
lw_searchiterator_free_full (LwSearchIterator *iterator)
{
    if (iterator == NULL) return;

    if (iterator->search != NULL) lw_search_free (iterator->search);

    lw_searchiterator_free (iterator);
}


void
lw_searchiterator_free (LwSearchIterator *iterator)
{
    if (iterator == NULL) return;
    g_hash_table_unref (iterator->historytable);

    memset (iterator, 0, sizeof(LwSearchIterator));

    g_free (iterator);
}


gint
lw_searchiterator_count (LwSearchIterator *iterator)
{
    //Sanity checks
    g_return_val_if_fail (iterator != NULL, 0);

    return iterator->count;
}

gint
lw_searchiterator_length (LwSearchIterator *iterator)
{
    //Sanity checks
    g_return_val_if_fail (iterator != NULL, 0);

    LwSearch *search = iterator->search;

    lw_search_lock (search);

    gint total = g_list_length (iterator->list);

    lw_search_unlock (search);

    return total;
}


gboolean
lw_searchiterator_finished (LwSearchIterator *iterator)
{
    //Sanity checks
    g_return_val_if_fail (iterator != NULL, FALSE);

    //Declarations
    LwSearch *search = iterator->search;
    LwSearchStatus status = lw_search_get_status (search);

    return (status == LW_SEARCHSTATUS_IDLE && (iterator->list == NULL || iterator->link->next == NULL));
}


gboolean
lw_searchiterator_empty (LwSearchIterator *iterator)
{
    return (iterator->list == NULL);
}


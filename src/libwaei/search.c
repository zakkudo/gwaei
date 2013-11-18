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
//! @file searchitem.c
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


//!
//! @brief Creates a new LwSearch object. 
//! @param query The text to be search for
//! @param dictionary The LwDictionary object to use
//! @param TARGET The widget to output the results to
//! @param error A GError to place errors into or NULL
//! @return Returns an allocated LwSearch object that should be freed with lw_search_free or NULL on error
//!
LwSearch* 
lw_search_new (LwDictionary       *dictionary, 
               LwMorphologyEngine *morphologyengine,
               const gchar        *QUERY,
               LwSearchFlag        flags)
{
    g_return_val_if_fail (dictionary != NULL, NULL);
    g_return_val_if_fail (morphologyengine != NULL, NULL);
    g_return_val_if_fail (QUERY != NULL, NULL);

    LwSearch *temp = g_new0 (LwSearch, 1);

    if (temp != NULL)
    {
      g_mutex_init (&temp->mutex);
      temp->status = LW_SEARCHSTATUS_IDLE;
      temp->dictionary = dictionary;
      temp->query = g_strdup(QUERY);
      temp->morphologyengine = morphologyengine;
      temp->flags = flags;
      temp->max = 500;
      temp->resulttable = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, (GDestroyNotify) g_list_free); 
      if (temp->resulttable == NULL) goto errored;

      g_object_ref (morphologyengine);
    }

    return temp;

errored:

    if (temp != NULL) lw_search_free (temp);

    return NULL;
}


//!
//! @brief Releases a LwSearch object from memory. 
//!
//! All of the various interally allocated memory in the LwSearch is freed.
//! The file descriptiors and such are made sure to also be closed.
//!
//! @param search The LwSearch to have it's memory freed.
//!
void 
lw_search_free (LwSearch* search)
{
    //Sanity check
    if (search == NULL) return;

    lw_search_cancel (search);
    if (search->query != NULL) g_free (search->query);
    if (lw_search_has_data (search)) lw_search_free_data (search);
    if (search->morphologyengine != NULL) g_object_unref (search->morphologyengine);
    if (search->resulttable != NULL) g_hash_table_unref (search->resulttable);

    g_mutex_clear (&search->mutex);

    memset (search, 0, sizeof(LwSearch));

    free (search);
}


void
lw_search_set_max_results (LwSearch *search, gint max)
{
    search->max = max;
}


//!
//! @brief comparison function for determining if two LwSearchs are equal
//! @param item1 The first search
//! @param item2 The second search
//! @returns Returns true when both items are either the same search or have similar innards
//!
gboolean 
lw_search_is_equal (LwSearch *item1, LwSearch *item2)
{
  //Declarations
  gboolean queries_are_equal;
  gboolean dictionaries_are_equal;
  //Sanity checks
  if (item1 == NULL)
  {
    return FALSE;
  }
  if (item2 == NULL)
  {
    return FALSE;
  }

  if (item1 == item2)
  {
    return TRUE;
  }

  //Initializations
  queries_are_equal = (item1->query != NULL && item2->query != NULL && strcmp(item1->query, item2->query) == 0);
  dictionaries_are_equal = (item1->dictionary == item2->dictionary);

  return (queries_are_equal && dictionaries_are_equal);
}


//!
//! @brief Used to set custom search data (Such as Window or TextView pointers)
//! @param search The LwSearch to set the data on.  It will free any previous data if it is already set.
//! @param data The data to set.
//! @param free_data_func A callback to use to free the data automatically as needed
//!
void 
lw_search_set_data (LwSearch *search, gpointer data, LwSearchDataFreeFunc free_data_func)
{
    //Sanity check
    g_assert (search != NULL);

    if (lw_search_has_data (search))
      lw_search_free_data (search);

    search->data = data;
    search->free_data_func = free_data_func;
}


//!
//! @brief to retieve custom search data (Such as Window or TextView pointers)
//! @param search The LwSearch object to retrieve the data on.
//! @returns A generic pointer to the data that should be cast.
//!
gpointer 
lw_search_get_data (LwSearch *search)
{
    //Sanity check
    g_assert (search != NULL);

    return search->data;
}


//!
//! @brief Frees the data on an LwSearch object if it exists
//! @param search The LwSearch to free the data on
//!
void 
lw_search_free_data (LwSearch *search)
{
    //Sanity check
    g_assert (search != NULL);

    if (search->free_data_func != NULL && search->data != NULL)
    {
      (search->free_data_func) (search->data);
    }

    search->data = NULL;
    search->free_data_func = NULL;
}


//!
//! @brief Returns true if the LwSearch had its data set
//! @param search An LwSearch to check for data
//! @returns Returns true if the data is not NULL
//!
gboolean 
lw_search_has_data (LwSearch *search)
{
    g_assert (search != NULL);

    return (search->data != NULL && search->free_data_func != NULL);
}


//!
//! @brief A wrapper around gmutex made for LwSearch objects
//! @param search An LwSearch to lock the mutex on
//!
void 
lw_search_lock (LwSearch *search)
{
  g_mutex_lock (&search->mutex);
}

//!
//! @brief A wrapper around gmutex made for LwSearch objects
//! @param search An LwSearch to unlock the mutex on
//!
void 
lw_search_unlock (LwSearch *search)
{
  g_mutex_unlock (&search->mutex);
}


void
lw_search_set_status (LwSearch       *search, 
                      LwSearchStatus  status)
{
    lw_search_lock (search);
      search->status = status;
    lw_search_unlock (search);
}


LwSearchStatus
lw_search_get_status (LwSearch *search)
{
    LwSearchStatus status;

    lw_search_lock (search);
      status = search->status;
    lw_search_unlock (search);

    return status;
}


static LwIndexFlag
_lw_search_get_index_flags (LwSearch *search)
{
    g_return_val_if_fail (search != NULL, 0);

    LwIndexFlag flags  = 0;
    if (search->flags & LW_SEARCH_FLAG_RAW) flags |= LW_INDEX_FLAG_RAW;
    if (search->flags & LW_SEARCH_FLAG_CASE_INSENSITIVE) flags |= LW_INDEX_FLAG_CASE_INSENSITIVE;
    if (search->flags & LW_SEARCH_FLAG_FURIGANA_INSENSITIVE) flags |= LW_INDEX_FLAG_FURIGANA_INSENSITIVE;
    if (search->flags & LW_SEARCH_FLAG_STEM_INSENSITIVE) flags |= LW_INDEX_FLAG_STEM_INSENSITIVE;

    return flags;
}


//!
//! @brief Preforms the brute work of the search
//!
//! THIS IS A PRIVATE FUNCTION. This function returns true until it finishes
//! searching the whole file.  It works in specified chunks before going back to
//! the thread to help improve speed.  
//!
//! @param data A LwSearch to search with
//! @return Returns true when the search isn't finished yet.
//!
static gpointer 
lw_search_stream_results_thread (gpointer data)
{
printf("BREAK lw_search_stream_results_thread\n");
    //Declarations
    LwSearch *search = NULL;
    LwProgress *progress = NULL;
    LwDictionary *dictionary = NULL;
    LwSearchFlag flags = 0;

    //Initializations
    search = LW_SEARCH (data);
    g_return_val_if_fail (search != NULL, NULL);
    progress = search->progress;
    dictionary = search->dictionary;
    flags = search->flags;

    //Index the dictionary if it isn't already
    if (flags & LW_SEARCH_FLAG_USE_INDEX)
    {
printf("BREAK lw_search_stream_results_thread trying to load index\n");
      if (!lw_dictionary_index_exists (dictionary)) lw_dictionary_index_create (dictionary, progress);
      if (lw_progress_should_abort (progress)) goto errored;
      lw_dictionary_index_load (dictionary, progress);
      if (lw_progress_should_abort (progress)) goto errored;
    }

    lw_search_lock (search);

    //Indexed search
    if (lw_dictionary_index_is_loaded (dictionary) && flags & LW_SEARCH_FLAG_USE_INDEX && !lw_util_is_regex_pattern (search->query, NULL))
    {
printf("BREAK lw_search_stream_results_thread index is loaded\n");
      LwMorphologyList *morphologylist = lw_search_get_query_as_morphologylist (search);

      if (search->resulttable != NULL) g_hash_table_unref (search->resulttable); search->resulttable = NULL;
      search->resulttable = lw_dictionary_index_search (dictionary, morphologylist, flags, progress);

      lw_morphologylist_free (morphologylist); morphologylist = NULL;
    }

    //Regex search
    else
    {
      if (search->resulttable != NULL) g_hash_table_unref (search->resulttable);
      search->resulttable = lw_dictionary_regex_search (dictionary, search->query, flags, progress);
    }

errored:

    search->status = LW_SEARCHSTATUS_FINISHING;
    search->thread = NULL;

    lw_search_unlock (search);

    return NULL;
}


//!
//! @brief Start a dictionary search
//! @param search a LwSearch argument to calculate results
//! @param create_thread Whether the search should run in a new thread.
//! @param exact Whether to show only exact matches for this search
//!
void 
lw_search_start (LwSearch   *search, 
                 LwProgress *progress,
                 gboolean    create_thread)
{
    //Sanity checks
    g_return_if_fail (search != NULL);
    g_return_if_fail (progress != NULL);

    //Initializations
    search->thread = NULL;
    search->status = LW_SEARCHSTATUS_SEARCHING;

    if (search->progress != NULL) lw_progress_free (search->progress);
    search->progress = progress;

    if (create_thread)
    {
      search->thread = g_thread_try_new (
        "libwaei-search",
        (GThreadFunc) lw_search_stream_results_thread, 
        (gpointer) search, 
        &progress->error
      );
      if (search->thread == NULL && progress->error != NULL)
      {
        g_warning ("Thread Creation Error: %s\n", progress->error->message);
        g_error_free (progress->error); progress->error = NULL;
      }
    }
    else
    {
      search->thread = NULL;
      lw_search_stream_results_thread ((gpointer) search);
    }
}


LwProgress*
lw_search_get_progress (LwSearch *search)
{
    //Sanity checks
    g_return_val_if_fail (search != NULL, NULL);

    return search->progress;
}


//!
//! @brief Uses a searchitem to cancel a window
//!
//! @param search A LwSearch to gleam information from
//!
void 
lw_search_cancel (LwSearch *search)
{
    if (search == NULL) return;

    if (search->progress != NULL) lw_progress_cancel (search->progress);
    lw_search_set_status (search, LW_SEARCHSTATUS_CANCELING);

    if (search->thread != NULL)
    {
      g_thread_join (search->thread);
      search->thread = NULL;
    }

    lw_search_set_status (search, LW_SEARCHSTATUS_IDLE);
}


LwSearchFlag
lw_search_get_flags (LwSearch *search)
{
    //Sanity checks
    g_return_val_if_fail (search != NULL, 0);

    return search->flags;
}


LwSearchFlag
lw_search_get_flags_from_preferences (LwPreferences *preferences)
{
    //Sanity checks
    g_return_val_if_fail (preferences != NULL, 0);

    //Declarations
    gboolean furigana_insensitive;
    gboolean case_insensitive;
    gboolean stem_insensitive;
    gint romaji_to_furigana;
    gboolean want_romaji_to_furigana_conv;
    gboolean index_results;
    gint32 flags;

    //Initializations
    furigana_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_FURIGANA_INSENSITIVE);
    case_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_CASE_INSENSITIVE);
    stem_insensitive = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_STEM_INSENSITIVE);
    romaji_to_furigana = lw_preferences_get_int_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_ROMAJI_TO_FURIGANA);
    want_romaji_to_furigana_conv = (romaji_to_furigana == 0 || (romaji_to_furigana == 2 && !lw_util_is_japanese_locale()));
    index_results = lw_preferences_get_boolean_by_schema (preferences, LW_SCHEMA_BASE, LW_KEY_INDEX_RESULTS);
    flags = LW_SEARCH_FLAG_RAW;

    if (furigana_insensitive) flags |= LW_SEARCH_FLAG_FURIGANA_INSENSITIVE;
    if (case_insensitive) flags |= LW_SEARCH_FLAG_CASE_INSENSITIVE;
    if (stem_insensitive) flags |= LW_SEARCH_FLAG_STEM_INSENSITIVE;
    if (want_romaji_to_furigana_conv) flags |= LW_SEARCH_FLAG_ROMAJI_TO_FURIGANA;
    if (index_results) flags |= LW_SEARCH_FLAG_USE_INDEX;

    return flags;
}


LwMorphologyList* 
lw_search_get_query_as_morphologylist (LwSearch* search)
{
    //Sanity checks
    g_return_val_if_fail (search != NULL, NULL);

    //Declarations
    LwMorphologyList *morphologylist;

    //Initializations

    morphologylist = lw_morphologyengine_analyze (search->morphologyengine, search->query, TRUE);

    return morphologylist;
}


gboolean
lw_search_has_results (LwSearch *search)
{
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init (&iter, search->resulttable);
    while (g_hash_table_iter_next (&iter, &key, &value))
    {
      if (value != NULL) return TRUE;
    }

    return FALSE;
}


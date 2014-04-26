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
//!  @file dictionary-index.c
//!
//!  @brief LwDictionary objects represent a loaded dictionary that the program
//!         can use to carry out searches.  You can uninstall dictionaries
//!         by using the object, but you cannot install them. LwDictInst
//!         objects exist for that purpose.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/dictionary-private.h>


static gint _sort_func (gconstpointer a, gconstpointer b, GHashTable *weight_table);


//Public methods/////////////////////////////////////////////////


GHashTable*
lw_dictionary_index_search (LwDictionary       *dictionary, 
                            LwMorphologyString *morphologystring,
                            LwIndexFlag         flags,
                            LwProgress         *progress)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);
    if (morphologylist == NULL) return NULL;
    g_return_val_if_fail (lw_dictionary_index_is_loaded (dictionary), NULL);

    //Declarations
    LwDictionaryPrivate *priv = dictionary->priv;
    LwIndex *index = priv->data.index;
    LwIndexFlag flag_list[] = {
      LW_INDEX_FLAG_RAW,
      LW_INDEX_FLAG_NORMALIZED,
      LW_INDEX_FLAG_STEM_INSENSITIVE,
      LW_INDEX_FLAG_CANONICAL
    };
    gint i = 0;
    LwIndexTableType type = 0;
    GHashTable *resulttable = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, (GDestroyNotify) g_list_free); 

    for (i = 0; i < G_N_ELEMENTS(flag_list); i++)
    {
      switch (flag_list[i]) {
        case LW_INDEX_FLAG_RAW:
          type = LW_INDEX_TABLE_RAW;
          break;
        case LW_INDEX_FLAG_NORMALIZED:
          type = LW_INDEX_TABLE_NORMALIZED;
          break;
        case LW_INDEX_FLAG_STEM_INSENSITIVE:
          type = LW_INDEX_TABLE_STEM;
          break;
        case LW_INDEX_FLAG_CANONICAL:
          type = LW_INDEX_TABLE_CANONICAL;
          break;
        default:
          g_assert_not_reached ();
          type = LW_INDEX_TABLE_INVALID;
          break;
      }

      GHashTable *weight_table = g_hash_table_new (g_direct_hash, g_direct_equal);
      const gchar *CATEGORY = lw_index_table_type_to_string (type);

      //The raw index concat only occurs of the morphology of the query is different than the raw form
      if (flags & flag_list[i])
      {
        GList *matchlist = lw_index_get_matches_for_morphologylist (index, type, morphologylist);
        GList *link = NULL;
        for (link = matchlist; link != NULL; link = link->next)
        {
          LwOffset offset = GPOINTER_TO_OFFSET (link->data);
          const gchar *HAYSTACK = lw_dictionary_get_string (dictionary, offset);
          gint score = lw_morphologylist_get_score (morphologylist, HAYSTACK);
          g_hash_table_insert (weight_table, LW_OFFSET_TO_POINTER (offset), GINT_TO_POINTER (score));
        }
        matchlist = g_list_sort_with_data (matchlist, (GCompareDataFunc) _sort_func, weight_table);
        
        g_hash_table_insert (resulttable, g_strdup (CATEGORY), matchlist);
      }

      if (weight_table != NULL) g_hash_table_unref (weight_table); weight_table = NULL;
    }

    return resulttable;

errored:

    if (resulttable != NULL) g_hash_table_unref (resulttable); resulttable = NULL;
*/
    return NULL;
}


gboolean
lw_dictionary_index_is_valid (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwIndex *index = NULL;
    LwDictionaryData *dictionarydata = NULL;
    gboolean valid = FALSE;

    //Initializations
    priv = dictionary->priv;
    index = priv->data.index;
    if (index == NULL) goto errored;
    dictionarydata = priv->data.data;
    if (dictionarydata == NULL) goto errored;

    valid = (g_strcmp0 (index->checksum, dictionarydata->checksum) == 0);

errored:

    return valid;
}


gboolean
lw_dictionary_index_exists (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);

    gchar *index_path = NULL;
    gboolean exists = FALSE;

    index_path = lw_dictionary_index_get_path (dictionary); if (index_path == NULL) goto errored;
    exists = lw_index_exists (index_path);

errored:

    if (index_path != NULL) g_free (index_path); index_path = NULL;

    return exists;
}


gchar*
lw_dictionary_index_get_path (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    gchar *directoryname = NULL;
    gchar *directory = NULL;
    const gchar *filename = NULL;
    gchar *path = NULL;
    GType type;

    //Initializations
    type = G_OBJECT_TYPE (dictionary);
    directoryname = lw_dictionary_get_directoryname (type); if (directoryname == NULL) goto errored;
    directory = lw_util_build_filename (LW_PATH_INDEX, directoryname); if (directory == NULL) goto errored;
    g_mkdir_with_parents (directory, 0755);
    filename = lw_dictionary_get_filename (dictionary); if (filename == NULL) goto errored;
    path = g_build_filename (directory, filename, NULL);

errored:

    if (directoryname != NULL) g_free (directoryname);
    if (directory != NULL) g_free (directory);

    return path;
}


gboolean
lw_dictionary_index_is_loaded (LwDictionary *dictionary)
{
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);

    LwDictionaryPrivate *priv = dictionary->priv;

    return (priv->data.index != NULL);
}


///!
///! @brief Loads the dictionary data and then creates an index file set that
///!        can be loaded with lw_dictionary_index_load().  Calling this functoin
///!        repeatedly will overwrite the previous index files.
///!
void
lw_dictionary_index_create (LwDictionary *dictionary, 
                            LwProgress   *progress)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (dictionary->priv != NULL);
    g_return_if_fail (dictionary->priv->data.morphology_engine != NULL);
    g_return_if_fail (dictionary->priv->filename != NULL);

    if (lw_progress_should_abort (progress)) return;

    //Declarations
    LwDictionaryPrivate *priv = dictionary->priv;
    gchar *dictionary_path = NULL;
    gchar *index_path = NULL;
    LwIndex *index = NULL;

    dictionary_path = lw_dictionary_get_path (dictionary); if (dictionary_path == NULL) goto errored;
    index_path = lw_dictionary_index_get_path (dictionary); if (index_path == NULL) goto errored;

    lw_progress_set_secondary_message (progress, "Creating index...");

    if (g_file_test (dictionary_path, G_FILE_TEST_IS_REGULAR) == FALSE) goto errored;

    //Load the dictionary data for the index
    if (priv->data == NULL)
    {
      priv->data = lw_dictionarydata_new ();
      lw_dictionarydata_create (priv->data, dictionary_path);
    }

    index = lw_index_new (priv->data.morphology_engine); if (index == NULL) goto errored;
    lw_index_create (index, priv->data, progress); if (lw_progress_should_abort (progress)) goto errored;
//    lw_index_validate_offsetlists (priv->data.index, priv->data);
    lw_index_write (index, index_path, progress); if (lw_progress_should_abort (progress)) goto errored;

errored:
    if (dictionary_path != NULL) g_free (dictionary_path); dictionary_path = NULL;
    if (index_path != NULL) g_free (index_path); index_path = NULL;
    if (index != NULL) lw_index_free (index); index = NULL;
    */
}


///!
///! @brief Loads the index if it isn't already loaded
///!
gboolean
lw_dictionary_index_load (LwDictionary *dictionary, 
                          LwProgress   *progress)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->data.morphology_engine != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->filename != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv = dictionary->priv;
    gchar *dictionary_path = NULL;
    gchar *index_path = NULL;

    dictionary_path = lw_dictionary_get_path (dictionary); if (dictionary_path == NULL) goto errored;
    index_path = lw_dictionary_index_get_path (dictionary); if (index_path == NULL) goto errored;

    if (g_file_test (dictionary_path, G_FILE_TEST_IS_REGULAR) == FALSE) goto errored;

    lw_progress_set_secondary_message (progress, "Loading index...");

    //Load the dictionary data for the index
    if (priv->data.data == NULL)
    {
      priv->data.data = lw_dictionarydata_new ();
      lw_dictionarydata_create (priv->data.data, dictionary_path);
    }

    //Unload the current index if it is invalid
    if (lw_dictionary_index_is_loaded (dictionary) && !lw_dictionary_index_is_valid (dictionary))
    {
      lw_index_free (priv->data.index); priv->data.index = NULL;
    }

    //Try reading the index file
    if (!lw_dictionary_index_is_loaded (dictionary) && lw_dictionary_index_exists (dictionary)) 
    {
      priv->data.index = lw_index_new (priv->data.morphology_engine);
      lw_index_read (priv->data.index, index_path, progress);
//      lw_index_validate_offsetlists (priv->data.index, priv->data);
    }

    //Check if it is valid
    if (lw_dictionary_index_is_loaded (dictionary) && !lw_dictionary_index_is_valid (dictionary))
    {
      lw_index_free (priv->data.index); priv->data.index = NULL;
    }

errored:
    if (dictionary_path != NULL) g_free (dictionary_path); dictionary_path = NULL;
    if (index_path != NULL) g_free (index_path); index_path = NULL;

    return (priv->data.index != NULL);
    */
    return FALSE;
}


//Private methods/////////////////////////////////////////////////


static gint
_sort_func (gconstpointer a, gconstpointer b, GHashTable *weight_table)
{
    //Declarations / Initializations
    LwOffset offset_a = GPOINTER_TO_OFFSET (a);
    gint weight_a = GPOINTER_TO_INT (g_hash_table_lookup (weight_table, a));
    LwOffset offset_b = GPOINTER_TO_OFFSET (b);
    gint weight_b = GPOINTER_TO_INT (g_hash_table_lookup (weight_table, b));

    if (weight_a < weight_b) return 1;
    else if (weight_a > weight_b) return -1;
    else
    {
      if (offset_a <  offset_b) return 1;
      if (offset_a >  offset_b) return -1;
      else return 0;
    }

    g_assert_not_reached ();
    return 0;
}

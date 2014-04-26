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
//! @file index.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


static void _load_offsetlist_into_hash (LwIndex *index, LwIndexTableType type, const gchar *KEY, GHashTable *table);
static LwOffset* _hash_to_offsetlist (GHashTable *table);
static void _lw_index_index_subkeys (LwIndex *index, const gchar *KEY);
static void _lw_index_index_subkeys_by_type (LwIndex *index, LwIndexTableType type, const gchar *KEY);


static gboolean
_lw_index_create_offset_exists (LwIndex          *index, 
                                LwIndexTableType  type, 
                                const gchar      *KEY, 
                                LwOffset          offset)
{
    //Sanity checks
    g_return_val_if_fail (index->table[type] != NULL, FALSE);
    g_return_val_if_fail (KEY != NULL, FALSE);

    //Declarations
    gint i = 0;
    LwOffset *data = NULL;
    GHashTable *table = NULL;

    //Initializations
    table = index->table[type];
    data = g_hash_table_lookup (table, KEY); if (data == NULL) return FALSE;

    for (i = 1; i < *data; i++)
    {
      if (data[i] == offset) return TRUE;
    }

    return FALSE;
}


///!
///! @brief Only to be used when creating an LwIndex
///!
static void
_lw_index_create_append_data_offset (LwIndex          *index, 
                                     LwIndexTableType  type, 
                                     const gchar      *KEY, 
                                     LwOffset          offset)
{
    //Sanity checks
    g_return_if_fail (index->table[type] != NULL);
    g_return_if_fail (KEY != NULL);
    if (_lw_index_create_offset_exists (index, type, KEY, offset)) return;

    //Declarantions
    LwOffset length = 0;
    LwOffset *original_data = NULL, *new_data = NULL;
    gchar *key = NULL;
    GHashTable *table = NULL;

    //Initializations
    table = index->table[type];
    key = g_strdup (KEY);
    original_data = g_hash_table_lookup (table, key);

    if (original_data == NULL) length = 2;
    else length = *original_data + 1;

    new_data = g_new0 (LwOffset, length);
    if (length > 2) memcpy(new_data, original_data, length * sizeof(LwOffset));
    new_data[0] = length; //The first value in the offset list is the length of the list
    new_data[length - 1] = offset;

    g_hash_table_replace (table, key, new_data);
}


LwOffsets*
_lw_index_get_data_offsets (LwIndex          *index,
                            LwIndexTableType  type, 
                            const gchar      *KEY)
{
    //Sanity checks
    g_return_val_if_fail (index != NULL, NULL);
    g_return_val_if_fail (index->table[type] != NULL, NULL);
    g_return_val_if_fail (KEY != NULL, NULL);

    //Declarations
    LwOffset *offsets;

    //Initializations
    offsets = g_hash_table_lookup (index->table[type], KEY);

    if (offsets != NULL) offsets++; //The first item is always the length of the array

    return offsets;
}


static LwOffset
_lw_index_get_data_offsets_length (LwIndex          *index, 
                                   LwIndexTableType  type, 
                                   const gchar      *KEY)
{
    //Sanity checks
    g_return_val_if_fail (index != NULL, 0);
    g_return_val_if_fail (index->table[type] != NULL, 0);
    g_return_val_if_fail (KEY != NULL, 0);

    //Declarations
    LwOffsets *offsets;
    LwOffset length;

    //Initializations
    offsets = g_hash_table_lookup (index->table[type], KEY);
    length = 0;

    if (offsets != NULL) length = *offsets - 1; //The first item is always the length of the array

    return (LwOffset) length;
}


///!
///! @brief Only to be used when creating the index
///!
static void
_lw_index_create_add_string (LwIndex     *index, 
                             const gchar *TEXT, 
                             LwOffset     offset)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (TEXT != NULL);
    g_return_if_fail (index->morphologyengine != NULL);
    g_return_if_fail (index->checksum == NULL); //You cannot add a string if the checksum has already been created

    //Declarations
    LwMorphology *morphology = NULL;
    LwMorphologyString *list = NULL;
    gchar *key = NULL;
    
    list = lw_morphologyengine_analyze (index->morphologyengine, TEXT, FALSE);
    while ((morphology = lw_morphologylist_read (list)) != NULL)
    {
      if (morphology->word != NULL && strlen(morphology->word) > 2) 
      {
        key = morphology->word; //Steals key
        _lw_index_create_append_data_offset (index, LW_INDEX_TABLE_RAW, key, offset);
        morphology->word = NULL;
      }
      if (morphology->normalized != NULL && strlen(morphology->normalized) > 2)
      {
        key = morphology->normalized; //Steals key
        _lw_index_create_append_data_offset (index, LW_INDEX_TABLE_NORMALIZED, key, offset);
        morphology->normalized = NULL;
      }
      if (morphology->stem != NULL && strlen(morphology->stem) > 2)
      {
        key = morphology->stem; //Steals key
        _lw_index_create_append_data_offset (index, LW_INDEX_TABLE_STEM, key, offset);
        morphology->stem = NULL;
      }
      if (morphology->canonical != NULL && strlen(morphology->canonical) > 2)
      {
        key = morphology->canonical; //Steals key
        _lw_index_create_append_data_offset (index, LW_INDEX_TABLE_CANONICAL, key, offset);
        morphology->canonical = NULL;
      }
    }
    lw_morphologylist_free (list); list = NULL;
    */
}


//!
//! @brief Indexes a file using the morphology engine.  A NULL path will create an empty index
//!
LwIndex* 
lw_index_new (LwMorphologyEngine *morphologyengine)
{
    //Sanity checks
    g_return_val_if_fail (morphologyengine != NULL, NULL);

    //Declarations
    LwIndex *index = NULL;
  
    //Initializations
    index = g_new0 (LwIndex, 1);
    index->morphologyengine = morphologyengine; g_object_ref (morphologyengine);

    if (index == NULL) goto errored;

errored:

    return index;
}


void
lw_index_free (LwIndex *index)
{
    //Sanity checks
    if (index == NULL) return;

    //Declarations
    gint i = 0;

    for (i = 0; i < TOTAL_LW_INDEX_TABLES; i++)
    {
      if (index->table[i] != NULL) g_hash_table_unref (index->table[i]);
      if (index->buffer[i] != NULL) g_free (index->buffer[i]);
    }
    if (index->morphologyengine != NULL) g_object_unref (index->morphologyengine);
    if (index->path != NULL) g_free (index->path);

    memset(index, 0, sizeof(LwIndex));

    g_free (index);
}

static void
_lw_index_deep_index (LwIndex          *index,
                      LwDictionaryData *dictionarydata,
                      LwProgress       *progress)
{
printf("BREAK _lw_index_deep_index\n");
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (dictionarydata != NULL);
    g_return_if_fail (index->checksum == NULL); //You cannot add a string if the checksum has already been created

    if (lw_progress_should_abort(progress)) return;

    GHashTable *table = NULL;
    GHashTableIter iter;
    gpointer key = NULL, value = NULL;
    gint i = 0;
    gint length = 0;
    gdouble fraction = 0.0;
    LwIndexTableType type = 0;

    //Get the total number of keys to analyse
    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      table = index->table[type]; if (table == NULL) continue;

      length += g_hash_table_size (table);
    }

    //Create subkey indexes
    for (type = 0; !lw_progress_should_abort(progress) && type < TOTAL_LW_INDEX_TABLES; type++)
    {
      table = index->table[type]; if (table == NULL) continue;

      g_hash_table_iter_init (&iter, table);
      while (g_hash_table_iter_next (&iter, &key, &value))
      {
        const gchar *KEY = key;
        _lw_index_index_subkeys (index, KEY);

        lw_progress_set_fraction (progress, i, length);

        if (lw_progress_should_abort (progress)) goto errored;

        i++;
      }
    }

    return;

errored:

    return;
}

//!
//! @brief Indexes a file using the morphology engine.  A NULL path will create an empty index
//!
void
lw_index_create (LwIndex          *index,
                 LwDictionaryData *dictionarydata,
                 LwProgress       *progress)
{
printf("BREAK lw_index_create\n");
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (index->morphologyengine != NULL);
    g_return_if_fail (dictionarydata != NULL);
    g_return_if_fail (progress != NULL);

    if (lw_progress_should_abort (progress)) return;

    //Declarations
    glong length = lw_dictionarydata_get_length (dictionarydata);
    gdouble fraction = 0.0;
    LwIndexTableType type = 0;
 
    //Clear the index tables
    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      if (index->table[type] != NULL) g_hash_table_unref (index->table[type]); index->table[type] = NULL;
      index->table[type] = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free); 
    }

    //Clear the checksums
    for (type = 0; !lw_progress_should_abort (progress) && type < TOTAL_LW_INDEX_TABLES; type++)
    {
      if (index->buffer[type] != NULL) g_free (index->buffer[type]); index->buffer[type] = NULL;
    }
    if (index->checksum != NULL) g_free ((gchar*)index->checksum); index->checksum = NULL;

    //Parse the data
    const gchar *BUFFER = lw_dictionarydata_get_buffer (dictionarydata);
    do {
      LwOffset offset = lw_dictionarydata_get_offset (dictionarydata, BUFFER);

      _lw_index_create_add_string (index, BUFFER, offset);

      lw_progress_set_fraction (progress, offset, length);

      if (lw_progress_should_abort (progress)) goto errored;

    } while ((BUFFER = lw_dictionarydata_buffer_next (dictionarydata, BUFFER)) != NULL) ;

    //_lw_index_deep_index (index, dictionarydata, progress);

    if (lw_progress_should_abort (progress)) goto errored;

    //Set the checksums
    for (type = 0; !lw_progress_should_abort (progress) && type < TOTAL_LW_INDEX_TABLES; type++)
    {
      index->buffer[type] = g_strdup (lw_dictionarydata_get_checksum (dictionarydata));
    }
    index->checksum = g_strdup (lw_dictionarydata_get_checksum (dictionarydata));

    return;

errored:

    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      if (index->table[type] != NULL) g_hash_table_unref (index->table[type]); index->table[type] = NULL;
      index->table[type] = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free); 
    }
}


static gboolean
_remove_func (gpointer key,
              gpointer value,
              gpointer data)
{
    return (GPOINTER_TO_INT (value) < GPOINTER_TO_INT (data));
}


static gboolean
_hash_remove_func (gpointer key, 
                   gpointer value,
                   gpointer data)
{
    gint min_weight = GPOINTER_TO_INT (data);
    gint actual_weight = GPOINTER_TO_INT (value);

    return (actual_weight >= min_weight);
}


static void
_concat_hash_func (GHashTable *table1, GHashTable *table2)
{
  GHashTableIter iter;
  gpointer key, value1, value2;

  g_hash_table_iter_init (&iter, table2);
  while (g_hash_table_iter_next (&iter, &key, &value2))
  {
    if (g_hash_table_contains (table1, key))
    {
      value1 = g_hash_table_lookup (table1, key);
      value1 = GINT_TO_POINTER (GPOINTER_TO_INT (value1) + GPOINTER_TO_INT (value2));
      g_hash_table_insert (table1, key, value1);
    }
    else
    {
      g_hash_table_insert (table1, key, value2);
    }
  }
}


static void
_lw_index_concat_masterkey_to_subkey (LwIndex          *index,
                                      LwIndexTableType  type,
                                      const gchar      *MASTERKEY,
                                      const gchar      *SUBKEY)
{
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (MASTERKEY != NULL);
    g_return_if_fail (SUBKEY != NULL);

    //Declarations
    LwOffset* offsetlist = NULL;
    GHashTable *table = NULL;

    //Initializations
    table = g_hash_table_new (g_direct_hash, g_direct_equal); if (table == NULL) goto errored;
    _load_offsetlist_into_hash (index, type, MASTERKEY, table);
    _load_offsetlist_into_hash (index, type, SUBKEY, table);

    offsetlist = _hash_to_offsetlist (table);
    gchar *key = g_strdup (SUBKEY);

    g_hash_table_replace (index->table[type], key, offsetlist);

errored:

    if (table != NULL) g_hash_table_unref (table); table = NULL;
}


///!
///! @brief This allows searching for fish, and getting bigfish back if the morphology parser doesn't catch it
///!
static void
_lw_index_index_subkeys_by_type (LwIndex          *index, 
                                 LwIndexTableType  type, 
                                 const gchar      *KEY)
{
    //Sanity checks
    g_return_val_if_fail (index != NULL, NULL);
    g_return_val_if_fail (KEY != NULL, NULL);

    //Declarations
    gchar *key_copy = NULL;
    gchar *ptr = NULL;
    const gchar *subkey = NULL;

    //Initializations
    key_copy = g_strdup (KEY); if (key_copy == NULL) goto errored;
    ptr = key_copy; if (*ptr != '\0') ptr = g_utf8_next_char (ptr);

    //Search for substrings by removing the front characters
    while (ptr != NULL && *ptr != '\0')
    {
      ptr = g_utf8_next_char (ptr); subkey = ptr;
      GUnicodeScript script = g_unichar_get_script (g_utf8_get_char (subkey));
      gboolean has_relevance = (*subkey != '\0' && (g_utf8_strlen (subkey, 9) > 2 || script == G_UNICODE_SCRIPT_HAN));
      if (has_relevance && g_hash_table_contains (index->table[type], subkey))
      {
        _lw_index_concat_masterkey_to_subkey (index, type, KEY, subkey);
      }
    }

    //Search for substring by removing the back characters
    subkey = key_copy;
    GUnicodeScript script = g_unichar_get_script (g_utf8_get_char (subkey));

    while (ptr > subkey) {
      ptr = g_utf8_prev_char (ptr); *ptr = '\0';
      gboolean has_relevance = (*subkey != '\0' && (g_utf8_strlen (subkey, 9) > 2 || script == G_UNICODE_SCRIPT_HAN));
      if (has_relevance && g_hash_table_contains (index->table[type], subkey))
      {
        _lw_index_concat_masterkey_to_subkey (index, type, KEY, subkey);
      }
    }

errored:

    if (key_copy != NULL) g_free (key_copy); key_copy = NULL;
}


static void
_lw_index_index_subkeys (LwIndex          *index, 
                         const gchar      *KEY)
{
    //Sanity checks
    g_return_val_if_fail (index != NULL, NULL);
    g_return_val_if_fail (KEY != NULL, NULL);

    //Declarations
    LwIndexTableType  type = 0;
    
    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      _lw_index_index_subkeys_by_type (index, type, KEY);
    }
}


static LwOffset*
_hash_to_offsetlist (GHashTable *table)
{
    //Sanity checks
    g_return_val_if_fail (table != NULL, NULL);

    //Declarations
    LwOffset length = g_hash_table_size (table) + 1;
    LwOffset *offsetlist = g_new (LwOffset, length);
    gint i = 0;

    offsetlist[i++] = length;

    {
      GHashTableIter iter;
      gpointer key, value;

      g_hash_table_iter_init (&iter, table);
      while (g_hash_table_iter_next (&iter, &key, &value))
      {
        LwOffset offset = GPOINTER_TO_OFFSET (key);
        offsetlist[i++] = offset;
      }
    }
    
    return offsetlist;
}


static void
_load_offsetlist_into_hash (LwIndex          *index, 
                            LwIndexTableType  type, 
                            const gchar      *KEY, 
                            GHashTable       *table)
{
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (KEY != NULL);
    g_return_if_fail (table != NULL);

    //Declarations
    LwOffsets *offsets = _lw_index_get_data_offsets (index, type, KEY); if (offsets == NULL) goto errored;
    gint length = _lw_index_get_data_offsets_length (index, type, KEY);
    gint i = 0;

    while (i < length)
    {
      LwOffset offset = offsets[i];
      gpointer key = LW_OFFSET_TO_POINTER (offset);
      gint weight = GPOINTER_TO_INT (g_hash_table_lookup (table, key)) + 1;
      gpointer value = GINT_TO_POINTER (weight);

      g_hash_table_insert (table, key, value);
      i++;
    }

errored:
    
    return;
}


/*TODO
///!
///! @brief Returns a hash table of results based on a morphology query from a specific LwIndexTableType
///!
static GHashTable*
_lw_index_get_matches_for_morphology (LwIndex          *index,
                                      LwIndexTableType  type,
                                      LwMorphology     *morphology)
{
    //Sanity checks
    g_return_val_if_fail (index != NULL, NULL);
    g_return_val_if_fail (morphology != NULL, NULL);

    //Declarations
    gint length = 0;
    gint i = 0;
    gint j = 0;
    GHashTable *table = g_hash_table_new (g_direct_hash, g_direct_equal); if (table == NULL) goto errored;
    const gchar *keys[] = { NULL, NULL, NULL };

    //Initializations
    switch (type)
    {
      case LW_INDEX_TABLE_RAW:
        keys[0] = lw_morphology_get_raw (morphology);
        break;
      case LW_INDEX_TABLE_NORMALIZED:
        keys[0] = lw_morphology_get_normalized (morphology);
        keys[1] = lw_morphology_get_raw (morphology);
        break;
      case LW_INDEX_TABLE_STEM:
        keys[0] = lw_morphology_get_stem (morphology);
        keys[1] = lw_morphology_get_raw (morphology);
        break;
      case LW_INDEX_TABLE_CANONICAL:
        keys[0] = lw_morphology_get_canonical (morphology);
        keys[1] = lw_morphology_get_raw (morphology);
        break;
      default:
        g_assert_not_reached ();
        goto errored;
        break;
    }

    //Hash all of the offsets, keeping count
    for (i = 0; keys[i] != NULL; i++)
    {
      _load_offsetlist_into_hash (index, type, keys[i], table);
    }

errored:
  
    return table;
}
*/


static gboolean
_prune (gpointer key, gpointer value, gpointer data)
{
    if (data == NULL) return FALSE;
    GHashTable *table = (GHashTable*) data;
    return (g_hash_table_contains (table, key) == FALSE);
}


  /*TODO
///!
///! @brief Removes keys from the previous_table if they are not in the new table
///!
static void
_lw_index_prune_matches_for_morphology (LwIndex          *index,
                                        LwIndexTableType  type,
                                        LwMorphology     *morphology,
                                        GHashTable       *previous_matches)
{
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (morphology != NULL);
    g_return_if_fail (previous_matches != NULL);

    //Declarations
    GHashTable *table = NULL;

    //Initializations
    table = _lw_index_get_matches_for_morphology (index, type, morphology); if (table == NULL) goto errored;

    g_hash_table_foreach_remove (previous_matches, _prune, table);

errored:

    if (table != NULL) g_hash_table_unref (table); table = NULL;
}
    */


GList*
lw_index_get_matches_for_morphologylist (LwIndex            *index, 
                                         LwIndexTableType    type, 
                                         LwMorphologyString  *morphologystring)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (index != NULL, NULL);
    g_return_val_if_fail (morphologystring != NULL, NULL);
    if (lw_morphologylist_length (morphologystring) == 0) return NULL;

    //Declarations
    GHashTable *table = NULL;
    GList *keylist = NULL;
    GList *link = morphologystring->list;

    while (link != NULL)
    {
      LwMorphology *morphology = link->data;
      if (morphology != NULL)
      {
        if (table == NULL) //Builds the inital list
          table = _lw_index_get_matches_for_morphology (index, type, morphology);
        else //Removes any items that do not have each morphology item
          _lw_index_prune_matches_for_morphology (index, type, morphology, table);

      }
      link = link->next;
    }

    keylist = g_hash_table_get_keys (table);

errored:

    if (table != NULL) g_hash_table_unref (table); table = NULL;

    return keylist;
    */
    return NULL;
}


const const gchar*
_lw_index_table_type_to_string (LwIndexTableType type)
{
    const gchar* suffixes[] = {
      "raw",
      "normalized",
      "stem",
      "canonical",
    };

    g_return_val_if_fail (G_N_ELEMENTS(suffixes) == TOTAL_LW_INDEX_TABLES, NULL);

    return suffixes[type];
}


void
lw_index_validate_offsetlists (LwIndex          *index,
                               LwDictionaryData *dictionarydata)
{
    //Declaraitons
    LwIndexTableType type = 0;
    printf("Validating offsets...\n");

    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      GHashTable *table = index->table[type]; if (table == NULL) continue;
      gint iternumber = 0;

      GHashTableIter iter;
      gpointer key, value;

      g_hash_table_iter_init (&iter, table);
      while (g_hash_table_iter_next (&iter, &key, &value))
      {
        LwOffset *offsetlist = value;
        const gchar *KEY = key;
        LwOffset length = *offsetlist - 1;
        LwOffset* list = offsetlist + 1;
        gint i = 0;

        while (i < length)
        {
          LwOffset offset = list[i];
          const gchar *result = lw_dictionarydata_get_string (dictionarydata, offset);
          g_assert (result != NULL);
          i++;
        }
      }
    }
}


///!
///! @brief Writes a specific index hash to a file
///!
static gsize
_lw_index_write_by_type (LwIndex          *index, 
                         LwIndexTableType  type,
                         gsize             current_progress,
                         gsize             total_progress,
                         const gchar*      PATH, 
                         LwProgress       *progress)
{
    //Sanity checks
    g_return_val_if_fail (index != NULL, 0);
    g_return_val_if_fail (index->table[type] != NULL, 0);
    g_return_val_if_fail (index->checksum != NULL, 0);
    g_return_val_if_fail (PATH != NULL, 0);
    g_return_val_if_fail (progress != NULL, 0);

    //Declarations
    FILE *fd = NULL;
    guint i, size;
    gdouble fraction;
    GHashTableIter iter;
    gchar *key = NULL;
    LwOffset *offsets = NULL;
    LwOffset length;
    const gchar *suffix = _lw_index_table_type_to_string (type);
    gchar *path = NULL;

    //Initializations
    path = g_strjoin (".", PATH, suffix, NULL); if (path == NULL) goto errored;
    fd = g_fopen (path, "wb"); if (fd == NULL) goto errored;
    size = g_hash_table_size (index->table[type]);
    i = 0;

    //Write the checksum with terminating null character
    fwrite (index->checksum, sizeof(gchar), strlen(index->checksum) + 1, fd);

    //Write the key/values
    g_hash_table_iter_init (&iter, index->table[type]);
    while (g_hash_table_iter_next (&iter, (gpointer*) &key, (gpointer*) &offsets))
    {
      length = *offsets;

      //Write key with terminating null character
      fwrite (key, sizeof(gchar), strlen(key) + 1, fd);

      //Write the offset data
      fwrite (offsets, sizeof(LwOffset), length, fd);

      //Update progress
      i++;
      lw_progress_set_fraction (progress, i + current_progress, total_progress);
    }

errored:

    if (path != NULL) g_free (path); path = NULL;
    if (fd != NULL) fclose(fd); fd = NULL;

    return current_progress + size;
}


///!
///! @brief Writes the all hashes used for the index to respecitive files
///!
void
lw_index_write (LwIndex     *index, 
                const gchar *PATH, 
                LwProgress  *progress)
{
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (progress != NULL);
    g_return_if_fail (index->checksum != NULL);

    //Declaraitons
    GHashTable *table = NULL;
    LwIndexTableType type = 0;
    gsize current_progress = 0;
    gsize total_progress = 0;

    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      table = index->table[type]; if (table == NULL) continue;
      total_progress += g_hash_table_size (table);
    }

    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      current_progress = _lw_index_write_by_type (index, type, current_progress, total_progress, PATH, progress);
    }
}


///!
///! @brief Gets the combined size off all of the index files
///!
static gsize
lw_index_get_length (LwIndex *index, const gchar *PATH)
{
    //Sanity checks
    g_return_if_fail (index != NULL);
    g_return_if_fail (PATH != NULL);

    //Declarations
    gchar *path = NULL;
    gsize length = 0;
    FILE *fd = NULL;
    LwIndexTableType type = 0;

    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      const gchar *SUFFIX = _lw_index_table_type_to_string (type);
      path = g_strjoin (".", PATH, SUFFIX, NULL); if (path == NULL) goto errored;
      fd = g_fopen (path, "r"); if (fd == NULL) goto errored;

      fseek(fd, 0, SEEK_END);
      length += ftell(fd);
  
      fclose (fd); fd = NULL;
      g_free (path); path = NULL;
    }

errored:

    if (path != NULL) g_free (path); path = NULL;
    if (fd != NULL) fclose (fd); fd = NULL;

    return length;
}


static gsize
_lw_index_read_by_type (LwIndex          *index, 
                        LwIndexTableType  type,
                        gsize             current_progress,
                        gsize             total_progress,
                        const gchar      *PATH, 
                        LwProgress       *progress)
{
    //Sanity checks
    g_return_val_if_fail (index != NULL, 0);
    g_return_val_if_fail (PATH != NULL, 0);
    g_return_val_if_fail (progress != NULL, 0);
    if (index->buffer[type] != NULL) return 0;

    //Declarations
    GHashTable *table = NULL;
    gchar *buffer = NULL;
    gchar *ptr = NULL;
    const gchar *checksum = NULL;
    gsize length = 0;
    gdouble fraction;
    const gchar *SUFFIX = _lw_index_table_type_to_string (type);
    gchar *path = NULL;

    //Initializations
    path = g_strjoin (".", PATH, SUFFIX, NULL); if (path == NULL) goto errored;
    if ((table = g_hash_table_new (g_str_hash, g_str_equal)) == NULL) goto errored;
    if (!g_file_get_contents (path, &buffer, &length, NULL)) goto errored;
    ptr = buffer;

    //Get the checksum
    checksum = ptr;

    //Advance passed the checksum
    while (*ptr != '\0' && ptr - buffer < length) ptr++; ptr++;
    if (ptr - buffer >= length || g_utf8_validate (checksum, -1, NULL) == FALSE) goto errored;

    do {
      //Get the key
      gchar *key = ptr;
      while (*ptr != '\0' && ptr - buffer < length) ptr++; ptr++;
      if (ptr - buffer >= length || g_utf8_validate(key, -1, NULL) == FALSE) goto errored;

      //Get the value
      LwOffsets *offsets = (LwOffsets*) ptr;
      ptr += (*offsets * sizeof(LwOffset));
      if (ptr - buffer > length) goto errored; 

      //Insert and Update
      g_hash_table_insert (table, key, offsets);

      //Update progress
      lw_progress_set_fraction (progress, ptr - buffer + current_progress, total_progress);
      
    } while (ptr - buffer < length);

    //Cleanup
    if (index->buffer[type] != NULL) g_free (index->buffer[type]);
    index->buffer[type] = buffer; buffer = NULL;

    if (index->table[type] != NULL) g_hash_table_unref (index->table[type]);
    index->table[type] = table; table = NULL;

    if (index->path != NULL) g_free (index->path);
    index->path = g_strdup (PATH); 

    index->checksum = checksum;

    if (path != NULL) g_free (path); path = NULL;

    return current_progress + length;

errored:

    g_warning ("Index file is invalid for PATH \"%s\" with type \"%d\".\n", path, type);

    if (path != NULL) g_free (path); path = NULL;
    if (buffer != NULL) g_free (buffer); buffer = NULL;
    if (table != NULL) g_hash_table_unref (table); table = NULL;

    return current_progress + length;
}


///!
///! @brief Loads the supported index files into the LwIndex
///! 
void
lw_index_read (LwIndex     *index, 
               const gchar *PATH, 
               LwProgress  *progress)
{
    g_return_if_fail (index != NULL);
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (progress != NULL);

    //Declarations
    LwIndexTableType type = 0;
    gsize current_progress = 0;
    gsize total_progress = lw_index_get_length (index, PATH);

    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      current_progress = _lw_index_read_by_type (index, type, current_progress, total_progress, PATH, progress);
    }
}


gboolean
lw_index_are_equal (LwIndex *index1, 
                    LwIndex *index2)
{
    //Declarations
    GHashTableIter iter;
    gchar *key1;
    LwOffset *value1, *value2;
    LwIndexTableType type = 0;

    for (type = 0; type < TOTAL_LW_INDEX_TABLES; type++)
    {
      g_hash_table_iter_init (&iter, index1->table[type]);
      while (g_hash_table_iter_next (&iter, (gpointer*) &key1, (gpointer*) &value1))
      {
        value2 = _lw_index_get_data_offsets (index2, type, key1);
        if (value2 == NULL) return FALSE;
        if (*value1 != *value2) return FALSE;
        if (memcmp(value1, value2, *value1) != 0) return FALSE;
      }
    }

    return TRUE;
}


gboolean
lw_index_exists (const gchar *PATH)
{
    //Sanity checks
    if (PATH == NULL) return FALSE;

    //Declarations
    gboolean all_exist = TRUE;
    gchar *path;
    const gchar *SUFFIX;
    LwIndexTableType type;

    for (type = 0; type < TOTAL_LW_INDEX_TABLES && all_exist; type++)
    {
      SUFFIX = _lw_index_table_type_to_string (type);
      path = g_strjoin (".", PATH, SUFFIX, NULL); 
      if (path != NULL)
      {
        if (g_file_test (path, G_FILE_TEST_IS_REGULAR) == FALSE) all_exist = FALSE;
        g_free (path); path = NULL;
      }
    }
    
    return all_exist;
}


const gchar*
lw_index_table_type_to_string (LwIndexTableType type)
{
    switch (type) {
      case LW_INDEX_TABLE_RAW:
        return "raw";
      case LW_INDEX_TABLE_NORMALIZED:
        return "normalized";
      case LW_INDEX_TABLE_STEM:
        return "stem";
      case LW_INDEX_TABLE_CANONICAL:
        return "canonical";
      default:
        g_assert_not_reached ();
    }
    g_assert_not_reached ();
}



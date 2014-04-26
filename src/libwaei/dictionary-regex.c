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
//!  @file dictionary-regex.c
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


GHashTable*
lw_dictionary_regex_search (LwDictionary  *self,
                            const gchar   *PATTERN,
                            LwIndexFlag    flags,
                            LwProgress    *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);
    g_return_val_if_fail (PATTERN != NULL, NULL);
    g_return_val_if_fail (LW_IS_PROGRESS (progress), NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryData *dictionarydata = NULL;
    GHashTable *resulttable = NULL;
    GList *matchlist = NULL;
    GRegex *regex = NULL;
    const gchar *BUFFER = NULL;
    LwOffset offset = 0;
    gint chunk = 0;
    LwOffset length = 0;
    const gchar *DICTIONARY_NAME = NULL;
    GError *error = NULL;

    //Initializations
    priv = self->priv;
    dictionarydata = priv->data;
    regex = g_regex_new (PATTERN, G_REGEX_OPTIMIZE, 0, &error); if (regex == NULL || error != NULL) goto errored;
    BUFFER = lw_dictionary_get_buffer (self); if (BUFFER == NULL) goto errored;
    length = lw_dictionarydata_get_length (priv->data);
    DICTIONARY_NAME = lw_dictionary_get_name (self);

    lw_progress_set_primary_message_printf (progress, "Searching %d self...", DICTIONARY_NAME);

    do {
      offset = lw_dictionarydata_get_offset (dictionarydata, BUFFER);
      if (g_regex_match (regex, BUFFER, 0, NULL) == TRUE)
      {
        matchlist = g_list_append (matchlist, LW_OFFSET_TO_POINTER (offset));
      }
      lw_progress_set_fraction (progress, offset, length);

/* TODO
      if (chunk > LW_SEARCH_MAX_CHUNK) 
      {
        chunk = 0;
        lw_search_unlock (search);
        g_thread_yield ();
        lw_search_lock (search);
      }
      chunk++;
*/
    } while ((BUFFER = lw_dictionarydata_buffer_next (dictionarydata, BUFFER)) != NULL && !lw_progress_should_abort (progress));

    resulttable = g_hash_table_new_full (g_str_hash, g_str_equal, (GDestroyNotify) g_free, (GDestroyNotify) g_list_free); 
    const gchar *CATEGORY = lw_index_table_type_to_string (LW_INDEX_TABLE_RAW);
    g_hash_table_insert (resulttable, g_strdup (CATEGORY), matchlist);

errored:

    if (progress != NULL)
    {
      lw_progress_set_error (progress, error);
      error = NULL;
    }
    else
    {
      g_clear_error (&error);
      error = NULL;
    }

    if (regex != NULL) g_regex_unref (regex); regex = NULL;

    return resulttable;
}


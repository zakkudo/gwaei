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
//! @file dictionarydata.c
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


LwDictionaryData*
lw_dictionarydata_new ()
{
    //Declarations
    LwDictionaryData *dictionarydata;

    //Initializations
    dictionarydata = g_new0 (LwDictionaryData, 1);

    return dictionarydata;
}


const gchar*
lw_dictionarydata_get_buffer (LwDictionaryData *dictionarydata)
{
    g_return_val_if_fail (dictionarydata != NULL, NULL);
    g_return_val_if_fail (dictionarydata->buffer != NULL, NULL);

    return dictionarydata->buffer;
}


//!
//! @brief Loads the dictionarydata from a file for use with a LwIndex
//!
void
lw_dictionarydata_create (LwDictionaryData *dictionarydata, 
                          const gchar      *PATH)
{
    //Sanity checks
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (dictionarydata != NULL);
    if (dictionarydata->checksum != NULL) return;
    if (dictionarydata->buffer != NULL) return;
    if (dictionarydata->path != NULL) return;

    //Declarations
    gchar *ptr = NULL;
    gsize length = 0;
 
    if (!g_file_test (PATH, G_FILE_TEST_IS_REGULAR)) goto errored;
    g_file_get_contents (PATH, &dictionarydata->buffer, &length, NULL); if (dictionarydata->buffer == NULL) goto errored;
    dictionarydata->checksum = g_compute_checksum_for_string (LW_INDEX_CHECKSUM_TYPE, dictionarydata->buffer, -1); if (dictionarydata->checksum == NULL) goto errored;
    dictionarydata->length = 0;

    for (ptr = dictionarydata->buffer; dictionarydata->length < length && *ptr != '\0'; ptr++)
    {
      if (*ptr == '\n')  
      {
        *(ptr++) = '\0';
        dictionarydata->length++;
      }
      dictionarydata->length++;
    }

    return;

errored:
  
    if (dictionarydata != NULL && dictionarydata->buffer != NULL) g_free (dictionarydata->buffer);
    if (dictionarydata != NULL && dictionarydata->path != NULL) g_free (dictionarydata->path);
    if (dictionarydata != NULL) lw_dictionarydata_free (dictionarydata);
}


const gchar*
lw_dictionarydata_get_string (LwDictionaryData *dictionarydata, 
                              LwOffset          offset)
{
    //Sanity checks
    g_return_val_if_fail (dictionarydata != NULL, NULL);
    g_return_val_if_fail (dictionarydata->buffer != NULL, NULL);
    g_return_val_if_fail (offset < dictionarydata->length, NULL);

    return (dictionarydata->buffer + offset);
}


//!
//! Returns a GList of const strings.  Free with g_list_free().
//!
GList*
lw_dictionarydata_get_strings (LwDictionaryData *dictionarydata, 
                               GList            *offsets)
{
    //Sanity checks
    g_return_val_if_fail (dictionarydata != NULL, NULL);
    g_return_val_if_fail (dictionarydata->buffer != NULL, NULL);

    //Declarations
    GList *results = NULL;
    GList *link = offsets;

    while (link != NULL)
    {
      LwOffset offset = GPOINTER_TO_OFFSET (link->data);
      const gchar* result = lw_dictionarydata_get_string (dictionarydata, offset);
      results = g_list_append (results, (gpointer) result);
      link = link->next;
    }    

    return results;
}


void
lw_dictionarydata_free (LwDictionaryData* dictionarydata)
{
    //Sanity checks
    if (dictionarydata == NULL) return;
    if (dictionarydata->buffer != NULL) g_free (dictionarydata->buffer); 
    if (dictionarydata->checksum != NULL) g_free (dictionarydata->checksum); 
    if (dictionarydata->path != NULL) g_free (dictionarydata->path); 

    memset(dictionarydata, 0, sizeof(LwDictionaryData));

    g_free (dictionarydata);
}


const gchar*
lw_dictionarydata_buffer_next (LwDictionaryData *dictionarydata, 
                               const gchar      *BUFFER)
{
    //Sanity checks
    g_return_val_if_fail (dictionarydata != NULL, NULL);
    g_return_val_if_fail (BUFFER != NULL, NULL);
    if (BUFFER > dictionarydata->buffer + dictionarydata->length) return NULL;
    if (BUFFER < dictionarydata->buffer, NULL) return NULL;

    BUFFER = BUFFER + strlen(BUFFER) + 1;

    if (BUFFER >= dictionarydata->buffer + dictionarydata->length) BUFFER = NULL;

    return BUFFER;
}


const gchar*
lw_dictionarydata_get_checksum (LwDictionaryData *dictionarydata)
{
    return dictionarydata->checksum;
}


LwOffset
lw_dictionarydata_get_length (LwDictionaryData *dictionarydata)
{
    return dictionarydata->length;
}


LwOffset
lw_dictionarydata_get_offset (LwDictionaryData *dictionarydata,
                              const gchar      *BUFFER)
{
    //Sanity checks
    g_return_val_if_fail (dictionarydata != NULL, NULL);
    g_return_val_if_fail (BUFFER != NULL, NULL);
    if (BUFFER > dictionarydata->buffer + dictionarydata->length) return 0;
    if (BUFFER < dictionarydata->buffer, NULL) return 0;

    return (BUFFER - dictionarydata->buffer);
}


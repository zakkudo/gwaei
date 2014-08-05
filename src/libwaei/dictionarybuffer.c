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


LwDictionaryBuffer*
lw_dictionarybuffer_new ()
{
    //Declarations
    LwDictionaryBuffer *self;

    //Initializations
    self = g_new0 (LwDictionaryBuffer, 1);

    return self;
}


const gchar*
lw_dictionarybuffer_start (LwDictionaryBuffer *self)
{
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->buffer != NULL, NULL);

    return self->buffer;
}


const gchar*
lw_dictionarybuffer_next (LwDictionaryBuffer *self, 
                          const gchar        *BUFFER)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    if (BUFFER == NULL) return lw_dictionarybuffer_start (self);
    if (BUFFER > self->buffer + self->length) return NULL;
    if (BUFFER < self->buffer, NULL) return NULL;

    BUFFER = BUFFER + strlen(BUFFER) + 1;

    if (BUFFER >= self->buffer + self->length) BUFFER = NULL;

    return BUFFER;
}


//!
//! @brief Loads the self from a file for use with a LwIndex
//!
void
lw_dictionarybuffer_create (LwDictionaryBuffer *self, 
                            const gchar        *PATH)
{
    //Sanity checks
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (self != NULL);
    if (self->checksum != NULL) return;
    if (self->buffer != NULL) return;
    if (self->path != NULL) return;

    //Declarations
    gchar *ptr = NULL;
    gsize length = 0;
 
    if (!g_file_test (PATH, G_FILE_TEST_IS_REGULAR)) goto errored;
    g_file_get_contents (PATH, &self->buffer, &length, NULL); if (self->buffer == NULL) goto errored;
    self->checksum = g_compute_checksum_for_string (LW_INDEX_CHECKSUM_TYPE, self->buffer, -1); if (self->checksum == NULL) goto errored;
    self->lines = g_new0(gchar*, length);

    {
      for (ptr = self->buffer; self->length < length && *ptr != '\0'; ptr++)
      {
        if (*ptr == '\n')  
        {
          *(ptr++) = '\0';
          self->lines[self->num_lines++] = ptr;
        }
      }
    }

    self->lines = g_realloc_n (self->lines, self->num_lines, sizeof(gchar*));

    return;

errored:
  
    lw_dictionarybuffer_free (self);
}


const gchar*
lw_dictionarybuffer_get_line (LwDictionaryBuffer *self, 
                              gint                number)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->buffer != NULL, NULL);
    g_return_val_if_fail (number < self->num_lines, NULL);

    return (self->lines[number]);
}


//!
//! Returns a GList of const strings.  Free with g_list_free().
//!
GList*
lw_dictionarybuffer_get_all_lines (LwDictionaryBuffer *self, 
                                   GList              *numbers)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->buffer != NULL, NULL);

    //Declarations
    GList *results = NULL;

    {
      GList *link = NULL;
      for (link = numbers; link != NULL; link = link->next)
      {
        gint number = GPOINTER_TO_INT (link->data);
        const gchar* BUFFER = lw_dictionarybuffer_get_line (self, number);
        results = g_list_prepend (results, (gpointer) BUFFER);
        link = link->next;
      }    
      results = g_list_reverse (results);
    }

    return results;
}


void
lw_dictionarybuffer_free (LwDictionaryBuffer* self)
{
    //Sanity checks
    if (self == NULL) return;
    g_free (self->buffer); 
    g_free (self->checksum); 
    g_free (self->path); 
    g_free (self->lines);

    memset(self, 0, sizeof(LwDictionaryBuffer));

    g_free (self);
}


const gchar*
lw_dictionarybuffer_get_checksum (LwDictionaryBuffer *self)
{
    return self->checksum;
}


LwOffset
lw_dictionarybuffer_length (LwDictionaryBuffer *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    return self->length;
}


LwOffset
lw_dictionarybuffer_get_offset (LwDictionaryBuffer *self,
                                const gchar      *BUFFER)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (BUFFER != NULL, NULL);
    if (BUFFER > self->buffer + self->length) return 0;
    if (BUFFER < self->buffer, NULL) return 0;

    return (BUFFER - self->buffer);
}


gchar * const *
lw_dictionary_lines (LwDictionary *self)
{
}


gint
lw_dictionary_num_lines (LwDictionary *self)
{
}


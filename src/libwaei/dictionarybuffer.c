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


GType
lw_dictionarybuffer_get_type ()
{
    static GType type = 0;

    if (type == 0)
    {
      type = g_boxed_type_register_static (
        "LwDictionaryBuffer",
        (GBoxedCopyFunc) lw_dictionarybuffer_ref,
        (GBoxedFreeFunc) lw_dictionarybuffer_unref
      );
    }
}


//!
//! @brief Loads the self from a file for use with a LwIndex
//!
void
lw_dictionarybuffer_create (LwDictionaryBuffer *self, 
                            const gchar        *PATH,
                            LwTokenizeFunc      tokenize)
{
    //Sanity checks
    g_return_if_fail (PATH != NULL);
    g_return_if_fail (self != NULL);
    if (self->checksum != NULL) return;
    if (self->buffer != NULL) return;
    if (self->path != NULL) return;

    //Declarations
    gsize length = 0;
    gchar **lines = NULL;
    gint num_lines = 0;
 
    if (!g_file_test (PATH, G_FILE_TEST_IS_REGULAR)) goto errored;
    g_file_get_contents (PATH, &self->buffer, &length, NULL); if (self->buffer == NULL) goto errored;
    self->checksum = g_compute_checksum_for_string (LW_INDEX_CHECKSUM_TYPE, self->buffer, -1); if (self->checksum == NULL) goto errored;

    lines = g_new0 (gchar*, length);
    self->tokens = g_new0 (gchar*, length);
    self->lines = g_new0 (gsize, length);

    //Make the lines separate strings
    {
      gchar *c = self->buffer;
      while (*c != '\0')
      {
        if (*c == '\n')  
        {
          gchar *e = g_utf8_next_char (c);
          *c = '\0';
          c = e;
          if (*c != '\n')
          {
            lines[num_lines++] = c;
          }
        }
        else
        {
          c = g_utf8_next_char (c);
        }
      }
    }

    self->lines = g_realloc_n (self->lines, num_lines, sizeof(gsize*));
    self->num_lines = num_lines;

    //Tokenize each line
    {
      gint i = 0;
      gint j = 0;
      gint num_tokens = 0;
      for (i = 0; i < num_lines; i++)
      {
        self->lines[i] = j;
        tokenize (lines[i], &self->tokens[j], &num_tokens);
        j += num_tokens + 1;
      }
    }

errored:

    g_free (lines); lines = NULL;

    return;
}


const gchar**
lw_dictionarybuffer_get_line (LwDictionaryBuffer *self, 
                              gint                number,
                              gint               *num_tokens_out)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->buffer != NULL, NULL);
    g_return_val_if_fail (number < self->num_lines, NULL);

    //Declarations
    gchar const * *tokens = NULL;
    gint max_tokens = 0;
    const gchar *c = NULL;
    const gchar *e = NULL;
    const gchar *p = NULL;
    gint num_tokens = 0;
/*TODO
    //Initializations
    max_tokens = self->length - (self->lines[number] - self->lines[0]);
    tokens = (gchar const * *) g_new0 (gchar*, max_tokens);
    c = self->lines[number];
    e = c + max_tokens;
    p = NULL;

    tokens[num_tokens++] = c;

    while (c < e)
    {
      if (*p == '\0' && *c != '\0') tokens[num_tokens++] = c;
      p = c;
      c = g_utf8_next_char (c);
    }

    if (num_tokens_out != NULL)
    {
      *num_tokens_out = num_tokens;
    }

    tokens = (gchar const **) g_realloc_n (tokens, max_tokens, sizeof(gchar*));

*/
    return tokens;
}


void
lw_dictionarybuffer_unref (LwDictionaryBuffer *self)
{
  //TODO
}


LwDictionaryBuffer*
lw_dictionarybuffer_ref (LwDictionaryBuffer *self)
{
  //TODO
  return NULL;
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


gsize
lw_dictionarybuffer_length (LwDictionaryBuffer *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    return self->length;
}


gsize
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
lw_dictionarybuffer_lines (LwDictionary *self)
{
  return NULL;
}


gint
lw_dictionarybuffer_num_lines (LwDictionary *self)
{
  return 0;
}


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
//! @file parseddictionary.c
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


LwParsedDictionary*
lw_parseddictionary_new (gsize num_lines)
{
    LwParsedDictionary *self = NULL;

    self = g_new0 (LwParsedDictionary, 1);
    self->lines = g_new0 (LwDictionaryLine, num_lines);
    self->num_lines = num_lines;

    return self;
}


GType
lw_parseddictionary_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      type = g_boxed_type_register_static (
        "LwParsedDictionary",
        (GBoxedCopyFunc) lw_parseddictionary_ref,
        (GBoxedFreeFunc) lw_parseddictionary_unref
      );
    }

    return type;
}


static void
lw_parseddictionary_free (LwParsedDictionary *self)
{
    //Sanity checks
    if (self == NULL) return; 

    {
      gint i = 0;
      for (i = 0; i < self->num_lines; i++)
      {
        lw_dictionaryline_clear (self->lines + i);
      }
    }

    g_free (self);
}


void lw_parseddictionary_unref (LwParsedDictionary *self)
{
    if (self == NULL) return;

    if (g_atomic_int_dec_and_test (&self->ref_count))
    {
      lw_parseddictionary_free (self);
    }
}

LwParsedDictionary* lw_parseddictionary_ref (LwParsedDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->ref_count);

    return self;
}


void
lw_parseddictionary_foreach (LwParsedDictionary            *self,
                             LwParsedDictionaryForeachFunc  func,
                             gpointer                       data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (func != NULL);

    //Declarations
    gint i = 0;
    gboolean has_error = FALSE;

    while (i < self->num_lines && !has_error)
    {
      has_error = !func(self, self->lines + i, data);
      i++;
    }
}


LwDictionaryLine*
lw_parseddictionary_get_line (LwParsedDictionary *self,
                              gsize               line_number)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (line_number > self->num_lines, NULL);

    //Declarations
    LwDictionaryLine *line = NULL;

    if (line_number < 0 || line_number >= self->num_lines) goto errored;

    line = self->lines + line_number;

errored:

    return line;
}


gsize
lw_parseddictionary_length (LwParsedDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    return self->num_lines;
}


struct _SerializedLengthData {
  gsize bytes;
};

static void
_get_serialized_length (LwParsedDictionary           *lines,
                        LwDictionaryLine             *line,
                        struct _SerializedLengthData *data)
{
    //Sanity checks
    g_return_if_fail (lines != NULL);
    g_return_if_fail (line != NULL);
    g_return_if_fail (data != NULL);

    data->bytes += lw_dictionaryline_get_serialized_length (line);
}


gsize
lw_parseddictionary_get_serialized_length (LwParsedDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    //Declarations
    struct _SerializedLengthData data = {
      .bytes = 0
    };

    //Initializations
    //[num_lines (gsize) [lines] ...]    
    data.bytes += sizeof(gsize);

    lw_parseddictionary_foreach (self, (LwParsedDictionaryForeachFunc) _get_serialized_length, &data);

    return data.bytes;
}


struct _SerializedDataData {
  guchar *buffer;
  guchar *c;
  gsize bytes_written;
  gsize bytes_left;
};

static gboolean
_get_serialized_data (LwParsedDictionary         *self,
                      LwDictionaryLine           *line,
                      struct _SerializedDataData *data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (line != NULL);
    g_return_if_fail (data != NULL);

    //Declarations
    gsize bytes_written = 0;
    gsize required_bytes = 0;
    gboolean has_error = FALSE;
    
    //Initializations
    required_bytes = lw_dictionaryline_get_serialized_length (line);
    if (required_bytes > data->bytes_left)
    {
      has_error = TRUE;
      goto errored;
    }
    bytes_written = lw_dictionaryline_serialize (line, data->c, data->bytes_left);
    data->bytes_left -= bytes_written;
    data->bytes_written += bytes_written;
    data->c += bytes_written;

errored:

    return !has_error;
}


gsize
lw_parseddictionary_serialize (LwParsedDictionary *self,
                               gchar              *preallocated_buffer,
                               gsize               buffer_length,
                               LwProgress         *progress)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (preallocated_buffer != NULL);
    g_return_if_fail (buffer_length > 0);

    //Declarations
    struct _SerializedDataData data = {
      .buffer = preallocated_buffer,
      .c = preallocated_buffer,
      .bytes_written = 0,
      .bytes_left = buffer_length
    };

    //Copy the number of LwDictionaryLines
    if ((data.c - data.buffer) + sizeof(gsize) <= data.bytes_left)
    {
      memcpy(data.c, &self->num_lines, sizeof(gsize));
      data.c += sizeof(gsize);
      data.bytes_left -= sizeof(gsize);
      data.bytes_written += sizeof(gsize);
    }

    lw_parseddictionary_foreach (self, (LwParsedDictionaryForeachFunc) _get_serialized_data, &data);

    return data.bytes_written;
}


LwParsedDictionary*
lw_parseddictionary_deserialize (LwParsedDictionary *self,
                                 const gchar        *contents_reference_point,
                                 const guchar       *serialized_data,
                                 LwProgress         *progress)
{
  /*TODO
  [num_lines (lwoffset)] [line]...
  type checking here
  */
}


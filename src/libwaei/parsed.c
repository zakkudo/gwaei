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
//! @file parsed.c
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


struct _SerializeData {
  gsize *buffer;
  gsize *write_pointer;
  gsize bytes_written;
  GError *error;
};

struct _DeserializeData {
  gchar const *serialized_data;
  gchar const *read_pointer;
  gsize bytes_read;
  GError *error;
};


struct _LwParsed {
  gsize num_lines; //! < The number of lines
  gchar *contents_reference_pointer; //! < The reference point of each strv
  gsize content_length;
  LwDictionaryLine *lines; //! < A set of categorized strvs
  gint ref_count;
};


LwParsed*
lw_parsed_new (gchar *contents,
               gsize  content_length)
{
    //Sanity checks
    g_return_val_if_fail (contents != NULL, NULL);
    g_return_val_if_fail (content_length > 0, NULL);

    //Declarations
    LwParsed *self = NULL;

    //Initializations
    self = g_new0 (LwParsed, 1);
    self->lines = NULL;
    self->num_lines = 0;
    self->contents_reference_pointer = contents;
    self->content_length = content_length;

    return self;
}


GType
lw_parsed_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      type = g_boxed_type_register_static (
        "LwParsed",
        (GBoxedCopyFunc) lw_parsed_ref,
        (GBoxedFreeFunc) lw_parsed_unref
      );
    }

    return type;
}


static void
lw_parsed_free (LwParsed *self)
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


void lw_parsed_unref (LwParsed *self)
{
    if (self == NULL) return;

    if (g_atomic_int_dec_and_test (&self->ref_count))
    {
      lw_parsed_free (self);
    }
}

LwParsed* lw_parsed_ref (LwParsed *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->ref_count);

    return self;
}


void
lw_parsed_foreach (LwParsed            *self,
                   LwParsedForeachFunc  func,
                   gpointer             data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (func != NULL);

    //Declarations
    gint i = 0;
    gboolean has_error = FALSE;

    while (i < self->num_lines && !has_error)
    {
      has_error = func(self, self->lines + i, data);
      i++;
    }
}


LwDictionaryLine*
lw_parsed_get_line (LwParsed *self,
                    gsize     line_number)
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


void
lw_parsed_set_lines (LwParsed         *self,
                     LwDictionaryLine *lines,
                     gsize             num_lines)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    if (lines == self->lines) goto errored;

    {
      gint i = 0;
      for (i = 0; i < self->num_lines; i++)
      {
        lw_dictionaryline_clear (self->lines + i);
      }
    } 
    g_free (self->lines); self->lines = 0;
    self->num_lines = 0;

    self->lines = lines;
    self->num_lines = num_lines;

errored:

    return;
}


gsize
lw_parsed_num_lines (LwParsed *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    return self->num_lines;
}


static gboolean
_serialize (LwParsed              *self,
            LwDictionaryLine      *dictionary_line,
            struct _SerializeData *data)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, TRUE);
    g_return_val_if_fail (dictionary_line != NULL, TRUE);
    g_return_val_if_fail (data != NULL, TRUE);

    //Declarations
    gsize bytes_written = 0;
    gchar *contents = NULL;
    gchar *write_pointer = NULL;

    //Initializations
    write_pointer = (gchar*) data->write_pointer;
    contents = self->contents_reference_pointer;
    bytes_written = lw_dictionaryline_serialize (dictionary_line, contents, write_pointer, &data->error);
    if (data->error != NULL)
    {
      goto errored;
    }
    data->bytes_written += bytes_written;
    write_pointer += bytes_written;
    data->write_pointer = (gsize*) write_pointer;

errored:

    return (data->error != NULL);
}


gsize
lw_parsed_serialize (LwParsed   *self,
                     gchar      *preallocated_buffer,
                     LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (preallocated_buffer != NULL);

    //Declarations
    struct _SerializeData data = {
      .buffer = (gsize*) preallocated_buffer,
      .write_pointer = (gsize*) preallocated_buffer,
      .bytes_written = 0
    };

    //Copy the number of LwDictionaryLines
    *(data.write_pointer++) = self->num_lines;
    data.bytes_written += sizeof(gsize);

    //Copy the individual seriallized LwDictionaryLine contents
    lw_parsed_foreach (self, (LwParsedForeachFunc) _serialize, &data);

errored:

    return data.bytes_written;
}


static gboolean
_deserialize (LwParsed                *self,
              LwDictionaryLine        *dictionary_line,
              struct _DeserializeData *data)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, TRUE);
    g_return_val_if_fail (dictionary_line != NULL, TRUE);
    g_return_val_if_fail (data != NULL, TRUE);

    //Declarations
    gsize bytes_read = 0;
    gchar const *contents = NULL;
    gchar const *read_pointer = NULL;

    //Initializations
    contents = self->contents_reference_pointer;
    read_pointer = data->read_pointer;
    bytes_read = lw_dictionaryline_deserialize_into (dictionary_line, read_pointer, contents, &data->error);
    if (data->error != NULL)
    {
      goto errored;
    }

errored:

    data->bytes_read += bytes_read;
    data->read_pointer += bytes_read;

    return (data->error != NULL);
}


gsize
lw_parsed_deserialize_into (LwParsed    *self,
                            const gchar *serialized_data,
                            gsize        serialized_length,
                            LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (serialized_data != NULL, 0);

    /*TODO
    [num_lines (lwoffset)] [line]...
    type checking here
    */

    //Declarations
    LwDictionaryLine *lines = NULL;
    gsize num_lines = 0;
    gsize bytes_read = 0;
    struct _DeserializeData data = {
      .serialized_data = serialized_data,
      .read_pointer = serialized_data,
      .bytes_read = 0,
      .error = NULL
    };

    //Initializations
    num_lines = *((gsize*) data.read_pointer);
    data.read_pointer += sizeof(gsize);
    lines = g_new0(LwDictionaryLine, num_lines);

    lw_parsed_foreach (self, (LwParsedForeachFunc) _deserialize, &data);
    if (data.error != NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, data.error);
        data.error = NULL;
      }
      g_clear_error (&data.error);
      goto errored;
    }

    lw_parsed_set_lines (self, lines, num_lines);
    lines = NULL;

errored:

    if (lines != NULL)
    {
      gint i = 0;
      for (i = 0; i < num_lines; i++)
      {
        lw_dictionaryline_clear (lines + i);
      }
      g_free (lines); lines = NULL;
    }

    return bytes_read;
}




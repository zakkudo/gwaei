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
//! @file dictionaryline.c
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


GQuark
lw_dictionaryline_error_quark ()
{
    return g_quark_from_static_string ("lw-dictionaryline-error");
}


struct _SerializeData {
  gchar const *contents;
  gsize *buffer;
  gsize *write_pointer;
  gsize bytes_written;
  GError *error;
};

struct _DeserializeData {
  gchar const *contents;
  gsize *serialized_data;
  gsize *read_pointer;
  gsize bytes_read;
  GError *error;
};


static gint
_compare (gconstpointer a,
          gconstpointer b,
          gpointer      user_data)
{
    return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}


void
lw_dictionaryline_init (LwDictionaryLine *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    lw_dictionaryline_clear (self);
    self->tree = g_tree_new_full (_compare, NULL, NULL, (GDestroyNotify) g_free);
}


void
lw_dictionaryline_init_static (LwDictionaryLine *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    lw_dictionaryline_clear (self);
    self->tree = g_tree_new_full (_compare, NULL, NULL, NULL);
}


void
lw_dictionaryline_clear (LwDictionaryLine *self)
{
    //Sanity checks
    if (self == NULL) return;

    //Declarations 
    GTree *tree = NULL;

    //Initializations
    tree = self->tree;
    self->tree = NULL;

    if (tree != NULL) g_tree_unref (tree);
    tree = NULL;
}


void
lw_dictionaryline_take_strv (LwDictionaryLine  *self,
                             gint               id,
                             gchar            **strv)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (id >= 0);

    if (strv == NULL)
    {
      g_tree_remove (self->tree, GINT_TO_POINTER (id));
    }
    else
    {
      g_tree_insert (self->tree, GINT_TO_POINTER (id), strv);
    }
}


gchar const * *
lw_dictionaryline_get_strv (LwDictionaryLine *self,
                            gint              id)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    if (self->tree == NULL)
    {
      return NULL;
    }

    return (g_tree_lookup (self->tree, GINT_TO_POINTER (id)));
}


gsize
lw_dictionaryline_get_serialized_length (LwDictionaryLine *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    //Declarations
    return lw_dictionaryline_serialize (self, NULL, NULL, NULL);
}


static gboolean
_serialize (gpointer               key,
            gpointer               value,
            struct _SerializeData *data)
{
    //Sanity checks
    g_return_val_if_fail (key != NULL, TRUE);
    g_return_val_if_fail (value != NULL, TRUE);

    //Declarations
    gint id = -1;
    gchar **strv = NULL;
    gboolean has_error = FALSE;
    gsize bytes_written = 0;

    //Initializations
    id = GPOINTER_TO_INT (key);
    if (id < 0) goto errored;
    strv = value;
    if (strv == NULL) goto errored;

    //Set the id
    if (data->write_pointer != NULL) *(data->write_pointer++) = (gsize) id;
    bytes_written += sizeof(gsize);

    //Set the string pointers
    while (*strv != NULL)
    {
      if (data->write_pointer != NULL) *(data->write_pointer++) = *(strv++) - data->contents;
      bytes_written += sizeof(gsize);
    }
    if (data->write_pointer != NULL) *(data->write_pointer++) = 0;
    bytes_written += sizeof(gsize);

errored:

    has_error = (bytes_written < (sizeof(gsize) * 2));
    data->bytes_written += bytes_written;

    return has_error;
}
                                            

gsize
lw_dictionaryline_serialize (LwDictionaryLine *self,
                             gchar            *contents_reference_pointer,
                             gchar            *preallocated_buffer,
                             GError           **error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (preallocated_buffer != NULL, 0);
    g_return_val_if_fail (preallocated_buffer != NULL && contents_reference_pointer == NULL, 0);
    if (error != NULL && *error != NULL) return 0;

    //Declarations
    gsize num_tokentypes = 0;
    struct _SerializeData data = {
      .contents = contents_reference_pointer,
      .buffer = (gsize*) preallocated_buffer,
      .write_pointer = (gsize*) preallocated_buffer,
      .bytes_written = 0,
      .error = NULL,
    };

    if (data.contents == NULL)
    {
      data.buffer = NULL;
      data.write_pointer = NULL;
    }

    //Initializations
    num_tokentypes = g_tree_nnodes (self->tree);

    //[num_tokentypes (gchar)]  [tokentypeid (strv)] [offsetbuffer (LwOffsetBuffer, lwoffset[])]... ]
    if (data.write_pointer != NULL) *(data.write_pointer++) = (gsize) num_tokentypes;
    data.bytes_written += sizeof(gsize);

    g_tree_foreach (self->tree, (GTraverseFunc) _serialize, &data);
    if (data.error != NULL)
    {
      goto errored;
    }

errored:

    if (data.error != NULL)
    {
      *error = data.error;
      data.error = NULL;
    }

    return data.bytes_written;
}


static gboolean
_merge_base_and_validate (gchar       **strv,
                          gchar const  *content)
{
    //Sanity checks
    g_return_val_if_fail (strv != NULL, FALSE);
    g_return_val_if_fail (content != NULL, NULL);

    //Declarations
    gint i = 0;
    gboolean is_valid = TRUE;
    while (strv[i] != NULL)
    {
      strv[i] = (gsize) strv[i] + (gchar*) content; //The value in strv is an offset, that needs the base to be a pointer again
      if (!g_utf8_validate (strv[i], -1, NULL)) is_valid = FALSE;
    }

    return is_valid;
}


gsize
lw_dictionaryline_deserialize_into (LwDictionaryLine *self,
                                    gchar const      *serialized_data,
                                    gchar const      *contents_reference_point,
                                    GError           **error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (serialized_data != NULL, 0);
    g_return_val_if_fail (contents_reference_point != NULL, 0);
    if (error != NULL && *error != NULL) return 0;

    lw_dictionaryline_init_static (self);

    //Declarations
    struct _DeserializeData data = {
      .contents = contents_reference_point,
      .serialized_data = (gsize*) serialized_data,
      .read_pointer = (gsize*) serialized_data,
      .bytes_read = 0,
      .error = NULL
    };
    gsize num_types = 0;

    num_types = *(data.read_pointer++);
    if (num_types < 1) goto errored;

    {
      //[num_tokentypes (gchar)]  [tokentypeid (strv)] [offsetbuffer (LwOffsetBuffer, lwoffset[])]... ]
      gsize token_type_id = 0;
      gchar **strv = NULL;
      while (num_types-- > 0)
      {
        token_type_id = (*data.read_pointer)++;
        strv = (gchar**) *data.read_pointer;
        if (!_merge_base_and_validate (strv, data.contents))
        {
          data.error = g_error_new (
            LW_DICTIONARYLINE_ERROR,
            LW_DICTIONARYLINE_ERRORCODE_DESERIALIZATION_ERROR,
            "The Contents or the serialized Data is invalid."
          );
          goto errored;
        }
        g_tree_insert (self->tree, GINT_TO_POINTER ((gint) token_type_id), strv);
        while (*data.read_pointer != 0) data.read_pointer++;
        data.read_pointer++;
      }
    }

errored:

    if (data.error != NULL)
    {
      *error = data.error;
      data.error = NULL;
    }

    return data.bytes_read;
}


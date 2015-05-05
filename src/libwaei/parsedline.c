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

/**
 * SECTION:parsedline
 * @short_description: A simple container for parsed data of a definition line
 * @title: LwParsedLine
 *
 * TODO
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/parsedline.h>


GQuark
lw_parsedline_error_quark ()
{
    return g_quark_from_static_string ("lw-parsedline-error");
}


struct _SerializeData {
  gchar const *contents;
  gchar *buffer;
  gchar *write_pointer;
  GError **error;
};

struct _DeserializeData {
  gchar const *contents;
  gchar *serialized_data;
  gchar *read_pointer;
  GError **error;
};


static gint
_compare (gconstpointer a,
          gconstpointer b,
          gpointer      user_data)
{
    return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}


void
lw_parsedline_init (LwParsedLine *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    lw_parsedline_clear (self);
    self->tree = g_tree_new_full (_compare, NULL, NULL, NULL);
}


void
lw_parsedline_init_full (LwParsedLine *self, GDestroyNotify destroy_notify_func)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    lw_parsedline_clear (self);
    self->tree = g_tree_new_full (_compare, NULL, NULL, destroy_notify_func);
}


void
lw_parsedline_clear (LwParsedLine *self)
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
lw_parsedline_take_strv (LwParsedLine  *self,
                         gint           id,
                         gchar        **strv)
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
lw_parsedline_get_strv (LwParsedLine *self,
                        gint          id)
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
lw_parsedline_get_serialized_length (LwParsedLine *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    //Declarations
    return lw_parsedline_serialize (self, NULL, NULL, NULL);
}


static gboolean
_serialize (LwParsedLine           * self,
            gint                     column,
            gchar                 ** strv,
            struct _SerializeData *  data)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, TRUE);
    g_return_val_if_fail (strv != NULL, TRUE);

    //Declarations
    gboolean has_error = FALSE;
    gint length = 0;
    gchar * offset = NULL;

    //Initializations
    length = g_strv_length (strv);

    //Set the column id
    if (data->buffer != NULL) memcpy(data->write_pointer, &column, sizeof(gint));
    data->write_pointer += sizeof(gint);

    if (data->buffer != NULL) memcpy(data->write_pointer, &length, sizeof(gint));
    data->write_pointer += sizeof(gint);

    //Set the string pointers
    while (*strv != NULL)
    {
      if (data->buffer != NULL)
      {
        offset = (gchar*) (*strv - data->contents);
        memcpy(data->write_pointer, &offset, sizeof(gchar*));
      }
      data->write_pointer += sizeof(gchar*);
      strv++;
    }

    if (data->buffer != NULL)
    {
      offset = (gchar*) (*strv);
      memcpy(data->write_pointer, &offset, sizeof(gchar*));
    }
    data->write_pointer += sizeof(gchar*);

errored:

    return has_error;
}
                                            

gsize
lw_parsedline_serialize (LwParsedLine  * self,
                         gchar const   * contents_reference_pointer,
                         gchar         * preallocated_buffer,
                         GError       ** error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    if (contents_reference_pointer != NULL || preallocated_buffer != NULL)
    {
      g_return_val_if_fail (preallocated_buffer != NULL, 0);
      g_return_val_if_fail (contents_reference_pointer != NULL, 0);
    }
    if (error != NULL && *error != NULL) return 0;

    //Declarations
    gint num_tokentypes = 0;
    struct _SerializeData data = {
      .contents = contents_reference_pointer,
      .buffer = preallocated_buffer,
      .write_pointer = preallocated_buffer,
      .error = error,
    };

    if (data.contents == NULL)
    {
      data.buffer = NULL;
      data.write_pointer = NULL;
    }

    //Initializations
    num_tokentypes = g_tree_nnodes (self->tree);

    //[num_tokentypes (gchar)]  [tokentypeid (strv)] [offsetbuffer (LwOffsetBuffer, lwoffset[])]... ]
    if (data.buffer != NULL) memcpy(data.write_pointer, &num_tokentypes, sizeof(gint));
    data.write_pointer += sizeof(gint);

    lw_parsedline_foreach (self, (LwParsedLineForeachFunc) _serialize, &data);
    if (data.error != NULL && *data.error != NULL) goto errored;

errored:

    return data.write_pointer - preallocated_buffer;
}


static gboolean
_merge_base_and_validate (gchar       ** strv,
                          gint           length,
                          gchar const  * content)
{
    //Sanity checks
    g_return_val_if_fail (strv != NULL, FALSE);
    g_return_val_if_fail (length > 0, FALSE);
    g_return_val_if_fail (content != NULL, NULL);

    //Declarations
    gint i = 0;
    gboolean is_valid = TRUE;

    //Initializations

    for (i = 0; i < length && is_valid; i++)
    {
      strv[i] = ((gsize) strv[i]) + ((gchar*) content); //The value in strv is an offset, that needs the base to be a pointer again
      if (!g_utf8_validate (strv[i], -1, NULL)) is_valid = FALSE;
    }

    return is_valid;
}


gsize
lw_parsedline_deserialize_into (LwParsedLine  *self,
                                gchar const   *serialized_data,
                                gchar const   *contents_reference_point,
                                GError       **error)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (serialized_data != NULL, 0);
    g_return_val_if_fail (contents_reference_point != NULL, 0);
    if (error != NULL && *error != NULL) return 0;

    lw_parsedline_init (self);

    //Declarations
    struct _DeserializeData data = {
      .contents = contents_reference_point,
      .serialized_data = (gchar*) serialized_data,
      .read_pointer = (gchar*) serialized_data,
      .error = error
    };
    gint num_types = 0;

    memcpy(&num_types, data.read_pointer, sizeof(gint));
    data.read_pointer += sizeof(gint);
    if (num_types < 1) goto errored;

    {
      //[num_tokentypes (gchar)]  [tokentypeid (strv)] [offsetbuffer (LwOffsetBuffer, lwoffset[])]... ]
      gint columnid = 0;
      gint num_tokens = 0;
      gchar *offsets = NULL;
      gchar ** strv = NULL;
      while (num_types-- > 0)
      {
        memcpy(&columnid, data.read_pointer, sizeof(gint));
        data.read_pointer += sizeof(gint);

        memcpy(&num_tokens, data.read_pointer, sizeof(gint));
        memset(data.read_pointer, 0, sizeof(gint)); //Keep from deserializing these values again if accidentally called twice
        data.read_pointer += sizeof(gint);
        
        if (num_tokens > 0)
        {
          strv = (gchar**) data.read_pointer;
          if (!_merge_base_and_validate (strv, num_tokens, data.contents))
          {
            g_set_error (
              error,
              LW_PARSEDLINE_ERROR,
              LW_PARSEDLINE_ERRORCODE_DESERIALIZATION_ERROR,
              "The Contents or the serialized Data is invalid."
            );
            goto errored;
          }
          g_tree_insert (self->tree, GINT_TO_POINTER (columnid), strv);
          data.read_pointer += sizeof(gchar*) * (num_tokens + 1);
        }
      }
    }

errored:

    return data.read_pointer - serialized_data;
}

static gboolean
_add_key (LwParsedLine  * self,
          gint            column,
          gchar        ** strv,
          GHashTable    * table)
{
    g_hash_table_add (table, GINT_TO_POINTER (column));
}

struct _ForeachData {
  LwParsedLine * self;
  LwParsedLineForeachFunc func;
  gpointer data;
};


static gboolean
_foreach (gpointer key,
          gpointer value,
          struct _ForeachData *data)
{
    //Sanity checks
    g_return_val_if_fail (data != NULL, TRUE);
    g_return_val_if_fail (data->self != NULL, TRUE);
    g_return_val_if_fail (data->func != NULL, TRUE);

    //Declarations
    gint column = 0;
    gchar ** strv = NULL;

    //Initializations
    column = GPOINTER_TO_INT (key);
    strv = (gchar**) value;

    return data->func(data->self, column, strv, data->data);
}


void
lw_parsedline_foreach (LwParsedLine * self, LwParsedLineForeachFunc func, gpointer data)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (func != NULL, NULL);

    struct _ForeachData foreach_data = {
      .func = func,
      .data = data,
      .self = self
    };

    g_tree_foreach (self->tree, (GTraverseFunc) _foreach, &foreach_data);
}


gchar **
lw_parsedline_lookup (LwParsedLine * self, gint column)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->tree != NULL, NULL);

    return g_tree_lookup (self->tree, GINT_TO_POINTER (column));
}


gint
lw_parsedline_num_columns (LwParsedLine * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (self->tree != NULL, 0);

    return g_tree_nnodes (self->tree);
}


void
lw_parsedline_assert_equals (LwParsedLine * self,
                             LwParsedLine * other)
{
    //Declarations
    GHashTable * table = NULL;
    gpointer key = NULL;
    gchar ** strv1 = NULL;
    gchar ** strv2 = NULL;
    gint i = 0;
    GHashTableIter iter = {0};

    //Initializations
    table = g_hash_table_new (g_direct_hash, g_direct_equal);
    if (table == NULL) goto errored;

    lw_parsedline_foreach (self, (LwParsedLineForeachFunc) _add_key, table);
    lw_parsedline_foreach (other, (LwParsedLineForeachFunc) _add_key, table);

    g_hash_table_iter_init (&iter, table);
    while (g_hash_table_iter_next (&iter, &key, NULL))
    {
        strv1 = g_tree_lookup (self->tree, key);
        strv2 = g_tree_lookup (other->tree, key);
        if (strv1 != NULL && strv2 != NULL)
        {
          for (i = 0; strv1[i] != NULL || strv2[i] != NULL; i++)
          {
            g_assert_cmpstr (strv1[i], ==, strv2[i]);
          }
        }
        else
        {
          g_assert_nonnull (strv1);
          g_assert_nonnull (strv2);
        }
    }

errored:

    if (table != NULL) g_hash_table_unref (table);
    table = NULL;
}


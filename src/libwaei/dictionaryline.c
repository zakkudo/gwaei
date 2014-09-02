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


gint
_compare (gpointer a, gpointer b)
{
  return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}


void
lw_dictionaryline_set_offsets (LwDictionaryLine *self,
                               gint              id,
                               lwoffset         *offsets)
{
  fix this
    //Sanity checks
    g_return_if_fail (self != NULL);

    if (self->tree == NULL)
    {
      self->tree = g_tree_new (_compare);
    }

    if (buffer == NULL)
    {
      g_tree_remove (self->data, GINT_TO_POINTER (id));
    }
    else
    {
      g_tree_insert (self->data, GINT_TO_POINTER (id), lw_offsetbuffer_clear (buffer, TRUE));
    }
}


lwoffset*
lw_dictionaryline_get_offsets (LwDictionaryLine *self,
                               gint              id)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    if (self->tree == NULL)
    {
      return NULL;
    }

    return (g_tree_lookup (self->data, GINT_TO_POINTER (id)));
}


gsize
lw_dictionaryline_get_serialization_length (LwDictionaryLine *self)
{
    g_return_val_if_fail (self != NULL, 0);

    return 0;
}
                                            

void
lw_dictionaryline_serialize (LwDictionaryLine *self,
                             guchar           *preallocated_buffer,
                             gsize             buffer_length)
{
  //Format
  //[num_tokentypes (guchar)]  [tokentypeid (gchar)] [offsetbuffer (LwOffsetBuffer, lwoffset[])]... ]
  return NULL;
}



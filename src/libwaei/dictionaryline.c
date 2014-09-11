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
_compare (gconstpointer a, gconstpointer b)
{
    return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}


void
lw_dictionaryline_init (LwDictionaryLine *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    if (self->tree == NULL)
    {
      self->tree = g_tree_new (_compare);
    }
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
    g_return_val_if_fail (self != NULL, 0);

    return 0;
}
                                            

gsize
lw_dictionaryline_serialize (LwDictionaryLine *self,
                             gchar            *preallocated_buffer,
                             gsize             buffer_length)
{
  //Format
  //[num_tokentypes (gchar)]  [tokentypeid (gchar)] [offsetbuffer (LwOffsetBuffer, lwoffset[])]... ]
  return 0;
}



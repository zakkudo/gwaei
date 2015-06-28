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
//! @file indexed.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>

#include <libwaei/gettext.h>
#include <libwaei/serializable.h>
#include <libwaei/indexed.h>

#include <libwaei/indexed-private.h>

G_DEFINE_TYPE (LwIndexed, lw_indexed, LW_TYPE_SERIALIZABLE)

LwIndexed*
lw_indexed_new (LwParsed *parsed)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (parsed), NULL);

    //Declarations
    LwIndexed *self = NULL;
    LwIndexedPrivate *priv = NULL;

    //Initializations
    self = LW_INDEXED (g_object_new (LW_TYPE_INDEXED, NULL));
    priv = self->priv;
    priv->parsed = g_object_ref (parsed);

    return self;
}


static void 
lw_indexed_init (LwIndexed *self)
{
    self->priv = LW_INDEXED_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwIndexedPrivate));
}


static void 
lw_indexed_finalize (GObject *object)
{
    //Declarations
    LwIndexed *self = NULL;
    LwIndexedPrivate *priv = NULL;

    //Initializations
    self = LW_INDEXED (object);
    priv = self->priv;

    if (priv->parsed != NULL) 
    {
      g_object_unref (priv->parsed);
      priv->parsed = NULL;
    }

    G_OBJECT_CLASS (lw_indexed_parent_class)->finalize (object);
}


static void
lw_indexed_class_init (LwIndexedClass *klass)
{
    //Declarations
    GObjectClass *object_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_indexed_finalize;
}


gsize
lw_indexed_get_serialized_length (LwIndexed   *self,
                                  LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    return lw_indexed_serialize (self, NULL, progress);
}


gsize
lw_indexed_serialize (LwIndexed   *self,
                       gchar      *preallocated_buffer,
                       LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (preallocated_buffer != NULL);

    return 0;
}


gsize
lw_indexed_deserialize_into (LwIndexed  *self,
                            const gchar *serialized_data,
                            gsize        serialized_length,
                            LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (serialized_data != NULL, 0);

    return 0;
}




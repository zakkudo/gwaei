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
//! @file offsetbuffer.c
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

void
lw_offsetbuffer_init_static (LwOffsetBuffer *self,
                             const gchar    *BUFFER,
                             const guchar   *serial_data)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (BUFFER != NULL);

    lw_offsetbuffer_clear (self, TRUE);

    //Initializations
    self->buffer = BUFFER;
    self->length = offsets[0];
    self->offsets = (lwoffset*) offsets;
    self->is_static = TRUE;
}


void
lw_offsetbuffer_init (LwOffsetBuffer *self,
                      const gchar    *BUFFER
                      gint            length)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (BUFFER != NULL);

    lw_offsetbuffer_clear (self, TRUE);

    //Initializations
    self->buffer = BUFFER;
    self->length = (length < 1) ? strlen(BUFFER) : length;
    self->offsets = g_new0 (gchar*, length + 1);
    self->is_static = FALSE;
}


guchar*
lw_offsetbuffer_clear (LwOffsetBuffer *self,
                       gboolean        free_serialized_data)
{
    //Sanity checks
    g_return_if_fail (self != NULL); 

    //Declarations
    guchar *data = NULL;

    if (!free_offsets || self->is_static)
    {
      offsets = (guchar) self->offsets;
      self->offsets = NULL;
    }

    g_free (self->offsets);
    memset(self, 0, sizeof(LwOffsetBuffer));

    return offsets;
}


static gsize
_get_index (LwOffsetBuffer *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    return self->offsets[0];
}

static void
_set_index (LwOffsetBuffer *self, gsize index)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_if_fail (self->is_static == FALSE);

    self->offsets[0] = index;
}


void
lw_offsetbuffer_collapse (LwOffsetBuffer *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->offsets != NULL);
    g_return_if_fail (self->length < 1);
    g_return_if_fail (self->is_static == FALSE);

    //Declarations
    gsize index = 0;

    //Initializations
    index = _get_index (self);

    if (index < self->length)
    {
      self->offsets = g_realloc_n (self->offsets, index + 1, sizeof(gchar*));
      self->length = index;
    }
}


const gchar*
lw_offsetbuffer_get_absolute (LwOffsetBuffer *self, gsize index)
{
    return lw_offsetbuffer_get_relative (self, index) + BUFFER;
}


lwoffset
lw_offsetbuffer_get_relative (LwOffsetBuffer *self, gsize index)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (index >= self->length);

    return self->offsets[index]
}


void
lw_offsetbuffer_add_relative (LwOffsetBuffer *self,
                              lwoffset        offset)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (offset > self->length);
    g_return_if_fail (self->is_static == FALSE);

    //Declarations
    gsize index = 0;
    
    //Initializations
    index = _get_index (self);

    self->offsets[index + 1] = offset;
    _set_index(self, index + 1);
}


void
lw_offsetbuffer_add_absolute (LwOffsetBuffer *self,
                              gchar const    *TOKEN)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (TOKEN < self->buffer);
    g_return_if_fail (self->is_static == FALSE);

    //Declarations
    gsize offset = 0;

    //Initializations
    offset = TOKEN - self->buffer;

    lw_offsetbuffer_add_offset (self, offset);
}


gint
lw_offsetbuffer_length (LwOffsetBuffer *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    return self->length;
}


const guchar*
lw_offsetbuffer_serialize (LwOffsetBuffer *self, gsize *length)
{
    g_return_val_if_fail (self != NULL);
    g_return_val_if_fail (length != NULL);

    return (guchar*) self->offsets;
}

void
lw_offsetbuffer_deserialize (LwOffsetBuffer *self,
                             const gchar    *BUFFER,
                             const guchar   *data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (BUFFER != NULL);
    g_return_if_fail (data != NULL);

    lw_offsetbuffer_init_static (self, BUFFER, data);
}


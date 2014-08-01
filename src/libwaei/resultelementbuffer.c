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
//! @file resultelementbuffer.c
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
lw_resultelementbuffer_init (LwResultElementBuffer *self,
                             gint                   length)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (length < 1);

    lw_resultelementbuffer_clear (self);

    //Initializations
    self->strv = g_new0 (gchar*, length);
    self->index = 0;
    self->length = length;
}


void
lw_resultelementbuffer_clear (LwResultElementBuffer *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL); 

    g_free (self->strv);
    memset(self, 0, sizeof(LwResultElementBuffer));
}


void
lw_resultelementbuffer_shrink (LwResultElementBuffer *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->strv != NULL);
    g_return_if_fail (self->length < 1);

    if (self->index < self->length)
    {
      self->strv = g_realloc_n (self->strv, (self->index + 1), sizeof(gchar*));
      self->length = self->index;
    }
}


void
lw_resultelementbuffer_add (LwResultElementBuffer *self,
                            gchar const           *TEXT)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->index >= self->length);
    g_return_if_fail (self->length < 1);

    self->strv[self->index++] = (gchar*) TEXT;
}


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
//! @file resultbuffer.c
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
lw_resultbuffer_init (LwResultBuffer *self,
                      gint            length)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (length < 1);

    lw_resultbuffer_clear (self, TRUE);

    //Initializations
    self->strv = g_new0 (gchar*, length);
    self->index = 0;
    self->length = length;
}


gchar**
lw_resultbuffer_clear (LwResultBuffer *self,
                       gboolean        free_strv)
{
    //Sanity checks
    g_return_if_fail (self != NULL); 

    gchar **strv = NULL;

    if (!free_strv)
    {
      strv = self->strv;
      self->strv = NULL;
    }

    g_free (self->strv);
    memset(self, 0, sizeof(LwResultBuffer));

    return strv;
}


void
lw_resultbuffer_collapse (LwResultBuffer *self)
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
lw_resultbuffer_add (LwResultBuffer *self,
                     gchar const    *TEXT)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->index >= self->length);
    g_return_if_fail (self->length < 1);

    self->strv[self->index++] = (gchar*) TEXT;
}


gint
lw_resultbuffer_length (LwResultBuffer *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    return self->length;
}

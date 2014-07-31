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
//! @file result.c
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


LwResult*
lw_result_new (const gchar *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (TEXT != NULL, NULL);

    //Declarations
    LwResult *self = NULL;

    //Initializations
    self = g_new0 (LwResult, 1);

    self->data = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, (GDestroyNotify) g_free);
    self->text = g_strdup (TEXT);
    self->length = strlen(TEXT);
    self->buffer = g_strdup (TEXT);
    self->buffer_end = self->buffer + self->length;

    return self;
}


void
lw_result_free (LwResult *self)
{
    //Sanity checks
    if (self == NULL) return;

    g_free (self->text);
    g_free (self->buffer);
    g_hash_table_unref (self->data); 

    memset(self, 0, sizeof(LwResult));

    g_free (self); 
}


gchar const *
lw_result_get_text (LwResult *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->text;
}


gchar *
lw_result_get_buffer (LwResult *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->buffer;
}


gboolean
lw_result_buffer_owns_text (LwResult    *self,
                            const gchar *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (TEXT != NULL, FALSE);

    //Declarations
    gboolean buffer_owns_text = NULL;

    //Initializations
    buffer_owns_text = (TEXT >= self->buffer && TEXT <= self->buffer_end);

    return buffer_owns_text;
}


gboolean
lw_result_buffer_owns_strv (LwResult  *self,
                            gchar    **TEXT)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (TEXT != NULL, FALSE);

    //Declarations
    gint i = 0;
    gboolean buffer_owns_strv = TRUE;

    for (i = 0; TEXT[i] != NULL && buffer_owns_strv; i++)
    {
      buffer_owns_strv = lw_result_buffer_owns_text (self, TEXT[i]);
    }

    return buffer_owns_strv;
}


void
lw_result_take (LwResult    *self,
                gchar const *KEY,
                gchar const *parsed_text)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (KEY != NULL);

    //Declarations
    gchar **strv = NULL;

    //Initializations
    
    if (parsed_text == NULL)
    {
      g_hash_table_remove (self->data, KEY);
    }
    else
    {
      strv = g_new0 (gchar*, 2);
      if (strv == NULL) goto errored;
      strv[0] = (gchar*) parsed_text;

      g_hash_table_insert (self->data, (gchar*) KEY, strv);

      strv = NULL;
      parsed_text = NULL;
    }


errored:

    g_free (strv); strv = NULL;

    return;
}


void
lw_result_take_strv (LwResult *self,
                     gchar const *KEY,
                     gchar const * * parsed_strv)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (KEY != NULL);
    if (parsed_strv == NULL) return;

    if (parsed_strv == NULL)
    {
      g_hash_table_remove (self->data, KEY);
    }
    else
    {
      g_hash_table_insert (self->data, (gchar*) KEY, parsed_strv);
    }
}


void
lw_result_take_elementbuffer (LwResult              *self,
                              const gchar           *KEY,
                              LwResultElementBuffer *elementbuffer)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (KEY != NULL);
    g_return_if_fail (elementbuffer != NULL);
    if (elementbuffer->strv == NULL) return;

    lw_result_take_strv (self, KEY, (gchar const**) elementbuffer->strv);

    elementbuffer->strv = NULL;
    lw_resultelementbuffer_clear (elementbuffer);
}


gchar const *
lw_result_get (LwResult *self,
               gchar const *KEY)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (KEY != NULL, NULL);

    //Declarations
    gchar **strv = NULL;
    gchar *value = NULL;

    //Initializations
    strv = g_hash_table_lookup (self->data, KEY);
    if (strv == NULL) goto errored;
    value = strv[0];

errored:

    return value;
}


gchar * const *
lw_result_get_strv (LwResult    *self,
                    gchar const *KEY)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (KEY != NULL, NULL);

    return (g_hash_table_lookup (self->data, KEY));
}


void
lw_result_init_elementbuffer (LwResult              *self,
                              LwResultElementBuffer *elementbuffer)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (elementbuffer != NULL);

    lw_resultelementbuffer_init (elementbuffer, self->length);
}



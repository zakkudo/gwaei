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


static void
_free_list (GList *list)
{
  g_list_free_full (list, g_free);
}


LwResult*
lw_result_new ()
{
    //Declarations
    LwResult *self = NULL;

    //Initializations
    self = g_new0 (LwResult, 1);

    self->data = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify) _free_list);
    self->lexicon = NULL;
}


void
lw_result_free (LwResult *self)
{
    if (self->data != NULL) g_hash_table_unref (self->data); self->data = NULL;
    if (self->lexicon != NULL) _free_list (self->lexicon); self->lexicon = NULL;
    g_free (self); self = NULL;
}


void
lw_result_take_data (LwResult    *self,
                     const gchar *KEY,
                     GList       *data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (KEY != NULL);

    g_hash_table_insert (self->data, (gchar*) KEY, data);
}

GList*
lw_result_get_data (LwResult    *self,
                    const gchar *KEY)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (KEY != NULL, NULL);

    return (g_hash_table_lookup (self->data, KEY));
}


void
lw_result_take_lexicon (LwResult *self,
                        GList    *lexicon)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    if (self->lexicon != NULL) g_list_free_full (self->lexicon, g_free);
    self->lexicon = lexicon;
}

GList*
lw_result_get_lexicon (LwResult *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    return self->lexicon;
}



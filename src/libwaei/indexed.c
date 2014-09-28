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

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


LwIndexed*
lw_indexed_new (LwParsed *parsed)
{
    //Sanity checks
    g_return_val_if_fail (parsed != NULL, NULL);

    //Declarations
    LwIndexed *self = NULL;

    //Initializations
    self = g_new0 (LwIndexed, 1);
    self->parsed = parsed;
    lw_parsed_ref (parsed);

    return self;
}


GType
lw_indexed_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      type = g_boxed_type_register_static (
        "LwIndexed",
        (GBoxedCopyFunc) lw_indexed_ref,
        (GBoxedFreeFunc) lw_indexed_unref
      );
    }

    return type;
}


static void
lw_indexed_free (LwIndexed *self)
{
    //Sanity checks
    if (self == NULL) return; 

    if (self->parsed != NULL) 
    {
      lw_parsed_unref (self->parsed);
    }

    memset(self, 0, sizeof(LwIndexed));

    g_free (self);
}


void lw_indexed_unref (LwIndexed *self)
{
    if (self == NULL) return;

    if (g_atomic_int_dec_and_test (&self->ref_count))
    {
      lw_indexed_free (self);
    }
}


LwIndexed*
lw_indexed_ref (LwIndexed *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->ref_count);

    return self;
}




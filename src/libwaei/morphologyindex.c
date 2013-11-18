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
//! @file morphology.c
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


LwMorphologyIndex*
lw_morphologyindex_new (gint         index, 
                        const gchar *BUFFER,
                        gint         start_offset, 
                        gint         end_offset)
{
    LwMorphologyIndex *morphologyindex = g_new (LwMorphologyIndex, 1);

    morphologyindex->index = index;
    morphologyindex->BUFFER = BUFFER;
    morphologyindex->start_offset = start_offset;
    morphologyindex->end_offset = end_offset;

    return morphologyindex;
}

void
lw_morphologyindex_free (LwMorphologyIndex *morphologyindex)
{
    memset (morphologyindex, 0, sizeof(LwMorphologyIndex));
    g_free (morphologyindex);
}


static gint
_sort_func (gconstpointer a, gconstpointer b)
{
    const LwMorphologyIndex *ia = a;
    const LwMorphologyIndex *ib = b;

    if (ia->start_offset < ib->start_offset) return -1;
    if (ia->start_offset > ib->start_offset) return 1;
    if (ia->start_offset == ib->start_offset) return 0;

    g_assert_not_reached ();
    return 0;
}



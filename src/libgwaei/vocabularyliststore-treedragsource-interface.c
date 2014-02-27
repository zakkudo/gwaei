/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Zachary Dovel. All Rights Reserved.

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
//! @file vocabularyliststore-treedragsource-interface.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularyliststore-private.h>


static gboolean
_row_draggable (GtkTreeDragSource *drag_source,
                GtkTreePath       *tree_path)
{
}


static gboolean
_drag_data_get (GtkTreeDragSource *drag_source,
                GtkTreePath       *tree_path,
                GtkSelectionData  *selection_data)
{
}


static gboolean
_drag_data_delete (GtkTreeDragSource *drag_soure,
                   GtkTreePath       *tree_path)
{
}


void
lgw_vocabularylistview_implement_actionable_interface (GtkTreeDragSourceIface *iface)
{
    iface->row_draggable = _row_draggable;
    iface->drag_data_get = _drag_data_get;
    iface->drag_data_delete = _drag_data_delete;
}


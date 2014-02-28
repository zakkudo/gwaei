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
    //Sanity checks 
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (drag_source), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    gint length = 0;
    gint index = 0;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (drag_source);
    length = lgw_vocabularyliststore_length (self);
    index = gtk_tree_path_get_indices (tree_path)[0];
    
    return (index > -1 && index < length);
}


static gboolean
_drag_data_get (GtkTreeDragSource *drag_source,
                GtkTreePath       *tree_path,
                GtkSelectionData  *selection_data)
{
    //Sanity checks 
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (drag_source), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    GtkTreeModel *tree_model = NULL;
    gint *indices = NULL;
    gint index = 0;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    gboolean success = FALSE;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (drag_source);
    tree_model = GTK_TREE_MODEL (self);
    index = gtk_tree_path_get_indices (tree_path)[0];
    vocabulary_word_store = lgw_vocabularyliststore_nth (self, index);
    if (vocabulary_word_store == NULL) goto errored;

    success = gtk_tree_set_row_drag_data (selection_data, tree_model, tree_path);
    if (success = FALSE) goto errored;

    //success = gtk_selection_data_set_text (selection_data, text, -1); TEST LATER

errored:

    return success;
}


static gboolean
_drag_data_delete (GtkTreeDragSource *drag_source,
                   GtkTreePath       *tree_path)
{
    //Sanity checks 
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (drag_source), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    gint *indices = NULL;
    gint index = 0;
    GList *removed = NULL;
    gboolean success = FALSE;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (drag_source);
    index = gtk_tree_path_get_indices (tree_path)[0];
    indices = g_new0 (gint, 2);
    if (indices == NULL) goto errored;

    indices[0] = index;
    indices[1] = -1;

    removed = lgw_vocabularyliststore_remove_all (self, indices);
    if (removed != NULL) success = TRUE;

errored:

    if (indices != NULL) g_free (indices); indices = NULL;
    if (removed != NULL) g_list_free (removed); removed = NULL;

    return success;
}


void
lgw_vocabularyliststore_implement_treedragsource_interface (GtkTreeDragSourceIface *iface)
{
    iface->row_draggable = _row_draggable;
    iface->drag_data_get = _drag_data_get;
    iface->drag_data_delete = _drag_data_delete;
}


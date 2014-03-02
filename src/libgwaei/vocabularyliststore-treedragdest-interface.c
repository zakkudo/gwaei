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
_drag_data_received (GtkTreeDragDest  *drag_dest,
                     GtkTreePath      *dest,
                     GtkSelectionData *selection_data)
{
  printf("_drag_data_received\n");
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (drag_dest), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    GtkTreeModel *tree_model = NULL;
    gboolean success = FALSE;
    GtkTreePath *src = NULL;
    GList *list = NULL;
    gint *indices = NULL;
    gint position = 0;
    gint depth = 0;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (drag_dest);
    tree_model = GTK_TREE_MODEL (self);
    indices = gtk_tree_path_get_indices_with_depth (dest, &depth);
    position = indices[0];

    success = gtk_tree_get_row_drag_data (selection_data, &tree_model, &src);
    if (success == FALSE) goto errored;

    if (LGW_IS_VOCABULARYLISTSTORE (tree_model) && depth == 1)
    {
      LgwVocabularyWordStore *vocabulary_word_store = NULL;
      vocabulary_word_store = lgw_vocabularyliststore_get_wordstore (self, src);
      if (vocabulary_word_store != NULL)
      {
        list = g_list_append (list, vocabulary_word_store);
        lgw_vocabularyliststore_insert_all (self, position, list);
      }
      printf("gtk_tree_drag_dest_drag_data_received: position: %d, depth %d\n", position, depth);
    }
    else if (LGW_IS_VOCABULARYWORDSTORE (tree_model) && depth == 2)
    {
      success = FALSE;
      //TODO
    }
    else
    {
      printf("gtk_tree_drag_dest_drag_data_received: failed\n");
      success = FALSE;
    }

errored:

    if (src != NULL) gtk_tree_path_free (src); src = NULL;
    if (list != NULL) g_list_free (list); list = NULL;

    return success;
}


static gboolean
_row_drop_possible (GtkTreeDragDest  *drag_dest,
                    GtkTreePath      *dest_path,
                    GtkSelectionData *selection_data)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (drag_dest), FALSE);
    if (drag_dest == NULL) return FALSE;
    if (selection_data == NULL) return FALSE;

    //Declarations
    LgwVocabularyListStore *self = NULL;
    GtkTreeModel *tree_model = NULL;
    GtkTreePath *tree_path = NULL;
    gboolean possible = FALSE;
    gint depth = 0;
    gint position = 0;
    gint *indices = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (drag_dest);
    tree_model = GTK_TREE_MODEL (self);
    indices = gtk_tree_path_get_indices_with_depth (dest_path, &depth);
    position = indices[0];

    possible = gtk_tree_get_row_drag_data (selection_data, &tree_model, &tree_path);
    if (!possible) goto errored;

    possible = (LGW_IS_VOCABULARYLISTSTORE (tree_model) || LGW_IS_VOCABULARYWORDSTORE (tree_model));
    if (!possible) goto errored;

/* DO NO DO THIS CHECK  I will make the GtkTreeView try depth 2 and then 1 sequencially
    if (LGW_IS_VOCABULARYLISTSTORE (tree_model) && depth != 1) possible = FALSE;
    if (!possible) goto errored;

    if (LGW_IS_VOCABULARYWORDSTORE (tree_model) && depth != 2) possible = FALSE;
    if (!possible) goto errored;
*/

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;

    printf("BREAK drop possible? possible: %d, position: %d, depth: %d\n", possible, position, depth);

    return possible;
}


void
lgw_vocabularyliststore_implement_treedragdest_interface (GtkTreeDragDestIface *iface)
{
    iface->drag_data_received = _drag_data_received;
    iface->row_drop_possible = _row_drop_possible;
}


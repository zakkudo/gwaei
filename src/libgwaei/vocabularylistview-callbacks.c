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
//! @file vocabularylistview-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularylistview-private.h>


void
lgw_vocabularylistview_selection_changed_cb (LgwVocabularyListView *self,
                                             GtkTreeSelection      *tree_selection)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    GList *vocabulary_word_stores = NULL;
    LgwVocabularyWordView *vocabulary_word_view = NULL;
    LgwActionable *actionable = NULL;

    //Initializations
    priv = self->priv;
    if (priv == NULL) goto errored;
    vocabulary_word_view = lgw_vocabularylistview_get_wordview (self);
    if (vocabulary_word_view == NULL) goto errored;
    vocabulary_word_stores = lgw_vocabularylistview_get_selected_wordstores (self);
    actionable = LGW_ACTIONABLE (self);

    lgw_vocabularywordview_set_wordstores (vocabulary_word_view, vocabulary_word_stores);

    lgw_actionable_sync_actions (actionable);

errored:

    if (vocabulary_word_stores != NULL) g_list_free (vocabulary_word_stores);

    return;
}


void
lgw_vocabularylistview_add_new_activated_cb (GSimpleAction *action,
                                             GVariant      *parameter,
                                             gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (data));

    //Declarations
    LgwVocabularyListView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (data);

    lgw_vocabularylistview_add_new (self);
}


void
lgw_vocabularylistview_delete_selected_activated_cb (GSimpleAction *action,
                                                     GVariant      *parameter,
                                                     gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (data));

    //Declarations
    LgwVocabularyListView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (data);

    lgw_vocabularylistview_delete_all_selected (self);
}


gboolean
lgw_vocabularylistview_focus_in_event_cb (LgwVocabularyListView *self,
                                          GdkEvent              *event,
                                          GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (self), FALSE);
    g_return_val_if_fail (GTK_IS_TREE_VIEW (inner_tree_view), FALSE);

    //Declarations
    LgwActionable *actionable = NULL;

    //Initializations
    actionable = LGW_ACTIONABLE (self);

    lgw_actionable_sync_actions (actionable);

    return FALSE;
}


gboolean
lgw_vocabularylistview_focus_out_event_cb (LgwVocabularyListView *self,
                                           GdkEvent              *event,
                                           GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (self), FALSE);
    g_return_val_if_fail (GTK_IS_TREE_VIEW (inner_tree_view), FALSE);

    //Declarations
    LgwActionable *actionable = NULL;

    //Initializations
    actionable = LGW_ACTIONABLE (self);

    lgw_actionable_sync_actions (actionable);

    return FALSE;
}


void
lgw_vocabularylistview_name_edited_cb (LgwVocabularyListView *self,
                                       gchar                 *path_string,
                                       gchar                 *new_text,
                                       GtkCellRendererText   *renderer)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    GtkTreeModel *tree_model = NULL;
    GtkTreePath *tree_path = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;
    GtkTreeIter iter;

    //Initializations
    vocabulary_list_store = lgw_vocabularylistview_get_liststore (self);
    tree_model = GTK_TREE_MODEL (vocabulary_list_store);
    tree_path = gtk_tree_path_new_from_string (path_string);
    if (tree_path == NULL) goto errored;
    vocabulary_word_store = lgw_vocabularyliststore_get_wordstore (vocabulary_list_store, tree_path);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    gtk_tree_model_get_iter_from_string (tree_model, &iter, path_string);

    lw_vocabulary_set_filename (vocabulary, new_text);

    g_signal_emit_by_name (
      G_OBJECT (vocabulary_list_store),
      "row-changed",
      tree_path,
      &iter,
      NULL
    );

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
}


void
lgw_vocabularylistview_connect_signals (LgwVocabularyListView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_SELECTION_CHANGED] == 0)
    {
        priv->data.signalid[SIGNALID_SELECTION_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->data.tree_selection),
          "changed",
          G_CALLBACK (lgw_vocabularylistview_selection_changed_cb),
          self
        );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "focus-in-event",
          G_CALLBACK (lgw_vocabularylistview_focus_in_event_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "focus-out-event",
          G_CALLBACK (lgw_vocabularylistview_focus_out_event_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_NAME_EDITED] == 0)
    {
      priv->data.signalid[SIGNALID_NAME_EDITED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.cell_renderer[CELLRENDERER_NAME]),
          "edited",
          G_CALLBACK (lgw_vocabularylistview_name_edited_cb),
          self
      );
    }
}


void
lgw_vocabularylistview_disconnect_signals (LgwVocabularyListView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_SELECTION_CHANGED] != 0)
    {
        if (priv->data.tree_selection != NULL) //NOTE apparently the tree selection object is disposed of early
        {
          g_signal_handler_disconnect (
            G_OBJECT (priv->data.tree_selection),
            priv->data.signalid[SIGNALID_SELECTION_CHANGED]
          );
        }
        priv->data.signalid[SIGNALID_SELECTION_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.tree_view),
        priv->data.signalid[SIGNALID_FOCUS_IN_EVENT]
      );
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = 0;
    }

    if (priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.tree_view),
        priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT]
      );
      priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] = 0;
    }

    if (priv->data.signalid[SIGNALID_NAME_EDITED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.cell_renderer[CELLRENDERER_NAME]),
        priv->data.signalid[SIGNALID_NAME_EDITED]
      );
      priv->data.signalid[SIGNALID_NAME_EDITED] = 0;
    }
}



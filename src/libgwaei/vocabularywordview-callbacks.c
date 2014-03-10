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
//! @file vocabularywordview.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularywordview-private.h>


void
lgw_vocabularywordview_selection_changed_cb (LgwVocabularyWordView *self,
                                             GtkTreeSelection      *tree_selection)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwActionable *actionable = NULL;

    //Initializations
    priv = self->priv;
    if (priv == NULL) goto errored;
    actionable = LGW_ACTIONABLE (self);

    lgw_actionable_sync_actions (actionable);

errored:

    return;
}

void
lgw_vocabularywordview_add_new_activated_cb (GSimpleAction *action,
                                             GVariant      *parameter,
                                             gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (data);

    lgw_vocabularywordview_add_new (self);
}

void
lgw_vocabularywordview_delete_selected_activated_cb (GSimpleAction *action,
                                                     GVariant      *parameter,
                                                     gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (data);

    lgw_vocabularywordview_delete_selected (self);
}


void
lgw_vocabularywordview_save_activated_cb (GSimpleAction *action,
                                          GVariant      *parameter,
                                          gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (data);

    lgw_vocabularywordview_save (self);
}


void
lgw_vocabularywordview_revert_activated_cb (GSimpleAction *action,
                                            GVariant      *parameter,
                                            gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (data);

    lgw_vocabularywordview_revert (self);
}


void
lgw_vocabularywordview_copy_selected_activated_cb (GSimpleAction *action,
                                                   GVariant      *parameter,
                                                   gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (data);

    lgw_vocabularywordview_copy_selected (self);
}


void
lgw_vocabularywordview_cut_selected_activated_cb (GSimpleAction *action,
                                                  GVariant      *parameter,
                                                  gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (data);

    lgw_vocabularywordview_cut_selected (self);
}


void
lgw_vocabularywordview_paste_activated_cb (GSimpleAction *action,
                                           GVariant      *parameter,
                                           gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDVIEW (data);

    lgw_vocabularywordview_paste (self);
}


gboolean
lgw_vocabularywordview_focus_in_event_cb (LgwVocabularyWordView *self,
                                          GdkEvent              *event,
                                          GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDVIEW (self), FALSE);
    g_return_val_if_fail (GTK_IS_TREE_VIEW (inner_tree_view), FALSE);

    //Declarations
    LgwActionable *actionable = NULL;

    //Initializations
    actionable = LGW_ACTIONABLE (self);

    lgw_actionable_sync_actions (actionable);

    return FALSE;
}


gboolean
lgw_vocabularywordview_focus_out_event_cb (LgwVocabularyWordView *self,
                                           GdkEvent              *event,
                                           GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDVIEW (self), FALSE);
    g_return_val_if_fail (GTK_IS_TREE_VIEW (inner_tree_view), FALSE);

    //Declarations
    LgwActionable *actionable = NULL;

    //Initializations
    actionable = LGW_ACTIONABLE (self);

    lgw_actionable_sync_actions (actionable);

    return FALSE;
}


void
lgw_vocabularywordview_drag_motion_cb (LgwVocabularyWordView *self,
                                      GdkDragContext         *drag_context,
                                      gint                    x,
                                      gint                    y,
                                      guint                   time,
                                      GtkTreeView            *inner_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    GtkWidget *source_widget = NULL;
    GdkAtom target;

    GtkTreePath *dest_path = NULL;
    GtkTreeViewDropPosition drop_position = -1;
    gboolean block = FALSE;
    gint depth = 0;

    //Initializations
    priv = self->priv;
    source_widget = gtk_drag_get_source_widget (drag_context);
    block = !gtk_tree_view_get_dest_row_at_pos (inner_tree_view, x, y, &dest_path, &drop_position);
    if (block) goto errored;
    if (dest_path == NULL) goto errored;
    target = gtk_drag_dest_find_target (GTK_WIDGET (inner_tree_view), drag_context, NULL);
    depth = gtk_tree_path_get_depth (dest_path);


    if (GTK_IS_TREE_VIEW (source_widget))
    {
      GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (source_widget));
      if (LGW_IS_VOCABULARYWORDSTORE (tree_model))
      {
        gtk_drag_highlight (GTK_WIDGET (inner_tree_view));
        if (drop_position == GTK_TREE_VIEW_DROP_INTO_OR_BEFORE)
          gtk_tree_view_set_drag_dest_row (inner_tree_view, dest_path, GTK_TREE_VIEW_DROP_BEFORE);
        else if (drop_position == GTK_TREE_VIEW_DROP_INTO_OR_AFTER)
          gtk_tree_view_set_drag_dest_row (inner_tree_view, dest_path, GTK_TREE_VIEW_DROP_AFTER);
      }
    }

    if (target == GDK_NONE)
    {
      gdk_drag_status (drag_context, 0, time);
    }
    else
    {
      priv->data.suggested_action = gdk_drag_context_get_suggested_action (drag_context);
      gtk_drag_get_data (GTK_WIDGET (inner_tree_view), drag_context, target, time);
    }

errored:

    if (dest_path != NULL) gtk_tree_path_free (dest_path); dest_path = NULL;
}


static gint
_get_insert_index (LgwVocabularyWordView *self, gint x, gint y)
{
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;
    GtkTreePath *tree_path = NULL;
    GtkTreeViewDropPosition drop_position = 0;
    gint index = -1;
    gint length = 0;

    //Initializations
    priv = self->priv;
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
    gtk_tree_view_get_dest_row_at_pos (priv->ui.tree_view, x, y, &tree_path, &drop_position);
    if (tree_path == NULL) goto errored;
    index = gtk_tree_path_get_indices (tree_path)[0];
    length = lw_vocabulary_length (vocabulary);

    if (drop_position == GTK_TREE_VIEW_DROP_INTO_OR_AFTER || drop_position == GTK_TREE_VIEW_DROP_AFTER)
    {
      index++;
    }

    if (index < 0 || index >= length) index = -1;

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;

    return index;
}


gboolean
lgw_vocabularywordview_drag_drop_cb (LgwVocabularyWordView *self,
                                     GdkDragContext        *drag_context,
                                     gint                   x,
                                     gint                   y,
                                     guint                  time,
                                     gpointer              *inner_text_view)
{
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    GtkTreeSelection *selection = NULL;
    GtkTreeModel *tree_model = NULL;
    GList *rows = NULL;
    gint success = FALSE;
    GtkWidget *source_widget = NULL;

    //Initializations
    source_widget = gtk_drag_get_source_widget (drag_context);
    if (!GTK_IS_TREE_VIEW (source_widget)) goto errored;
    tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (source_widget));
    if (tree_model == NULL) goto errored;
    
    if (LGW_IS_VOCABULARYWORDSTORE (tree_model))
    {
      LgwVocabularyWordStore *vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
      LwVocabulary *vocabulary = LW_VOCABULARY (tree_model);
      GList *words = lgw_vocabularywordview_get_selected_words (self);
      gint position = _get_insert_index (self, x, y);
      lw_vocabulary_insert_all (vocabulary, position, words);
      lgw_vocabularywordview_select_words (self, words);
      success = TRUE;
    }

errored:

    gtk_drag_finish (drag_context, success, FALSE, time);

    return TRUE;
}


void
lgw_vocabularywordview_drag_data_received_cb (LgwVocabularyWordView *self,
                                              GdkDragContext        *drag_context,
                                              gint                   x,
                                              gint                   y,
                                              GtkSelectionData      *selection_data,
                                              guint                  info,
                                              guint                  time,
                                              GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    GtkWidget *source_widget = NULL;
    GtkTreeModel *tree_model = NULL;
    GdkModifierType mask = 0;;

    //Initializations
    priv = self->priv;
    source_widget = gtk_drag_get_source_widget (drag_context);
    if (!GTK_IS_TREE_VIEW (source_widget)) goto errored;
    tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (source_widget));
    gdk_window_get_device_position (gtk_widget_get_window (GTK_WIDGET (inner_tree_view)), gdk_drag_context_get_device (drag_context), NULL, NULL, &mask);

    if (LGW_IS_VOCABULARYWORDSTORE (tree_model))
    {
      if (mask & GDK_CONTROL_MASK)
      {
        priv->data.suggested_action = GDK_ACTION_COPY;
      }
      else
      {
        priv->data.suggested_action = GDK_ACTION_MOVE;
      }
      gdk_drag_status (drag_context, priv->data.suggested_action, time);
    }
    else
    {
      gdk_drag_status (drag_context, 0, time);
    }

errored:

    return;
}
                  

void
lgw_vocabularywordview_drag_leave_cb (LgwVocabularyWordView *self,
                                      GdkDragContext        *drag_context,
                                      guint                  time,
                                      GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    gtk_drag_unhighlight (GTK_WIDGET (inner_tree_view));
}


void
lgw_vocabularywordview_connect_signals (LgwVocabularyWordView *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_SELECTION_CHANGED] == 0)
    {
        priv->data.signalid[SIGNALID_SELECTION_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->data.tree_selection),
          "changed",
          G_CALLBACK (lgw_vocabularywordview_selection_changed_cb),
          self
        );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "focus-in-event",
          G_CALLBACK (lgw_vocabularywordview_focus_in_event_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "focus-out-event",
          G_CALLBACK (lgw_vocabularywordview_focus_out_event_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DRAG_MOTION] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_MOTION] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-motion",
          G_CALLBACK (lgw_vocabularywordview_drag_motion_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DRAG_DROP] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_DROP] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-drop",
          G_CALLBACK (lgw_vocabularywordview_drag_drop_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DRAG_DATA_RECEIVED] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_DATA_RECEIVED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-data-received",
          G_CALLBACK (lgw_vocabularywordview_drag_data_received_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DRAG_LEAVE] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_LEAVE] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-leave",
          G_CALLBACK (lgw_vocabularywordview_drag_leave_cb),
          self
      );
    }
}


void
lgw_vocabularywordview_disconnect_signals (LgwVocabularyWordView *self)
{

    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;

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

    if (priv->data.signalid[SIGNALID_DRAG_MOTION] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.tree_view),
        priv->data.signalid[SIGNALID_DRAG_MOTION]
      );
      priv->data.signalid[SIGNALID_DRAG_MOTION] = 0;
    }

    if (priv->data.signalid[SIGNALID_DRAG_DROP] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.tree_view),
        priv->data.signalid[SIGNALID_DRAG_DROP]
      );
      priv->data.signalid[SIGNALID_DRAG_DROP] = 0;
    }

    if (priv->data.signalid[SIGNALID_DRAG_DATA_RECEIVED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.tree_view),
        priv->data.signalid[SIGNALID_DRAG_DATA_RECEIVED]
      );
      priv->data.signalid[SIGNALID_DRAG_DATA_RECEIVED] = 0;
    }

    if (priv->data.signalid[SIGNALID_DRAG_LEAVE] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.tree_view),
        priv->data.signalid[SIGNALID_DRAG_LEAVE]
      );
      priv->data.signalid[SIGNALID_DRAG_LEAVE] = 0;
    }
}


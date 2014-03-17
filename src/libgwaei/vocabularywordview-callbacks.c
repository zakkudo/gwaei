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
    lgw_vocabularywordview_sync_editable (self);

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
lgw_vocabularywordview_kanji_edited_cb (LgwVocabularyWordView *self,
                                        gchar                 *path_string,
                                        gchar                 *new_text,
                                        GtkCellRendererText   *renderer)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    GtkTreePath *tree_path = NULL;

    //Initializations
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    tree_path = gtk_tree_path_new_from_string (path_string);
    if (tree_path == NULL) goto errored;

    lgw_vocabularywordstore_set (vocabulary_word_store, tree_path, LW_WORD_FIELD_KANJI, new_text, -1);

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
}


void
lgw_vocabularywordview_reading_edited_cb (LgwVocabularyWordView *self,
                                          gchar                 *path_string,
                                          gchar                 *new_text,
                                          GtkCellRendererText   *renderer)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    GtkTreePath *tree_path = NULL;

    //Initializations
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    tree_path = gtk_tree_path_new_from_string (path_string);
    if (tree_path == NULL) goto errored;

    lgw_vocabularywordstore_set (vocabulary_word_store, tree_path, LW_WORD_FIELD_READING, new_text, -1);

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
}


void
lgw_vocabularywordview_definition_edited_cb (LgwVocabularyWordView *self,
                                             gchar                 *path_string,
                                             gchar                 *new_text,
                                             GtkCellRendererText   *renderer)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    GtkTreePath *tree_path = NULL;

    //Initializations
    vocabulary_word_store = lgw_vocabularywordview_get_wordstore (self);
    tree_path = gtk_tree_path_new_from_string (path_string);
    if (tree_path == NULL) goto errored;

    lgw_vocabularywordstore_set (vocabulary_word_store, tree_path, LW_WORD_FIELD_DEFINITION, new_text, -1);

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
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

    //Initializations
    priv = self->priv;
    source_widget = gtk_drag_get_source_widget (drag_context);
    block = !gtk_tree_view_get_dest_row_at_pos (inner_tree_view, x, y, &dest_path, &drop_position);
    if (block) goto errored;
    if (dest_path == NULL) goto errored;
    target = gtk_drag_dest_find_target (GTK_WIDGET (inner_tree_view), drag_context, NULL);


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


static gboolean
_move_from_wordstore (LgwVocabularyWordView *self,
                      GtkTreePath           *destination_tree_path,
                      GtkTreeView           *source_tree_view)
{
printf("BREAK0 _move_from_wordstore\n");
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));
    g_return_if_fail (GTK_IS_TREE_VIEW (source_tree_view));
    
    //Declarations
    GList *words = NULL;
    GList *copied_words;
    LgwVocabularyWordStore *destination_word_store = NULL;
    GtkTreeModel *source_tree_model = NULL;
    LgwVocabularyWordStore *source_word_store = NULL;
    GList *inserted_tree_paths = NULL;
    GList *original_tree_paths = NULL;
    GList *removed = NULL;
    gboolean success = FALSE;

    //Initializations
    words = lgw_treeview_get_selected_words (source_tree_view);
    if (words == NULL) goto errored;
    copied_words = g_list_copy_deep (words, (GCopyFunc) lw_word_copy, NULL);
    if (copied_words == NULL) goto errored;
    destination_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (destination_word_store == NULL) goto errored;
    source_tree_model = gtk_tree_view_get_model (source_tree_view);
    if (source_tree_model == NULL) goto errored;
    source_word_store = LGW_VOCABULARYWORDSTORE (source_tree_model);
    if (source_word_store == NULL) goto errored;

    inserted_tree_paths = lgw_vocabularywordstore_insert (destination_word_store, destination_tree_path, copied_words);
    if (inserted_tree_paths == NULL) goto errored;
    lw_vocabulary_remove_words (LW_VOCABULARY (source_word_store), words);
    lgw_vocabularywordview_select_words (self, copied_words);

    success = TRUE;

errored:

    g_list_free (words); words = NULL;
    g_list_free (copied_words); copied_words = NULL;
    g_list_free_full (inserted_tree_paths, (GDestroyNotify) gtk_tree_path_free); inserted_tree_paths = NULL;
    g_list_free_full (original_tree_paths, (GDestroyNotify) gtk_tree_path_free); original_tree_paths = NULL;
    g_list_free_full (removed, (GDestroyNotify) lw_word_free); removed = NULL;

    return FALSE;
}


static gboolean
_copy_from_wordstore (LgwVocabularyWordView *self,
                      GtkTreePath           *destination_tree_path,
                      GtkTreeView           *source_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));
    g_return_if_fail (GTK_IS_TREE_VIEW (source_tree_view));
    
    //Declarations
    GList *words = NULL;
    GList *copied_words;
    LgwVocabularyWordStore *destination_word_store = NULL;
    GtkTreeModel *source_tree_model = NULL;
    LgwVocabularyWordStore *source_word_store = NULL;
    GList *inserted_tree_paths = NULL;
    GList *original_tree_paths = NULL;
    GList *removed = NULL;
    gboolean success = FALSE;

    //Initializations
    words = lgw_treeview_get_selected_words (source_tree_view);
    if (words == NULL) goto errored;
    copied_words = g_list_copy_deep (words, (GCopyFunc) lw_word_copy, NULL);
    if (copied_words == NULL) goto errored;
    destination_word_store = lgw_vocabularywordview_get_wordstore (self);
    if (destination_word_store == NULL) goto errored;
    source_tree_model = gtk_tree_view_get_model (source_tree_view);
    if (source_tree_model == NULL) goto errored;
    source_word_store = LGW_VOCABULARYWORDSTORE (source_tree_model);
    if (source_word_store == NULL) goto errored;

    inserted_tree_paths = lgw_vocabularywordstore_insert (destination_word_store, destination_tree_path, copied_words);
    if (inserted_tree_paths == NULL) goto errored;
    lgw_vocabularywordview_select_words (self, copied_words);

    success = TRUE;

errored:

    g_list_free (words); words = NULL;
    g_list_free (copied_words); copied_words = NULL;
    g_list_free_full (inserted_tree_paths, (GDestroyNotify) gtk_tree_path_free); inserted_tree_paths = NULL;
    g_list_free_full (original_tree_paths, (GDestroyNotify) gtk_tree_path_free); original_tree_paths = NULL;
    g_list_free_full (removed, (GDestroyNotify) lw_word_free); removed = NULL;

    return FALSE;
}


gboolean
lgw_vocabularywordview_drag_drop_cb (LgwVocabularyWordView *self,
                                     GdkDragContext        *drag_context,
                                     gint                   x,
                                     gint                   y,
                                     guint                  time,
                                     gpointer              *inner_text_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (self));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    GtkTreeModel *tree_model = NULL;
    gboolean success = FALSE;
    GtkWidget *source_widget = NULL;
    GtkTreeView *source_tree_view = NULL;
    GtkTreePath *tree_path = NULL;
    GdkDragAction action = 0;

    //Initializations
    priv = self->priv;
    source_widget = gtk_drag_get_source_widget (drag_context);
    if (!GTK_IS_TREE_VIEW (source_widget)) goto errored;
    source_tree_view = GTK_TREE_VIEW (source_widget);
    if (source_tree_view == NULL) goto errored;
    tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (source_widget));
    if (tree_model == NULL) goto errored;
    tree_path = lgw_vocabularywordview_get_tree_path (self, drag_context, x, y);
    action = priv->data.suggested_action;
    
    if (LGW_IS_VOCABULARYWORDSTORE (tree_model))
    {
      if (action == GDK_ACTION_MOVE)
      {
        success = _move_from_wordstore (self, tree_path, source_tree_view);
      }
      else if (action == GDK_ACTION_COPY)
      {
        success = _copy_from_wordstore (self, tree_path, source_tree_view);
      }
    }

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;

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

    if (priv->data.signalid[SIGNALID_KANJI_EDITED] == 0)
    {
      priv->data.signalid[SIGNALID_KANJI_EDITED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.cell_renderer[CELLRENDERER_KANJI]),
          "edited",
          G_CALLBACK (lgw_vocabularywordview_kanji_edited_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_READING_EDITED] == 0)
    {
      priv->data.signalid[SIGNALID_READING_EDITED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.cell_renderer[CELLRENDERER_READING]),
          "edited",
          G_CALLBACK (lgw_vocabularywordview_reading_edited_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DEFINITION_EDITED] == 0)
    {
      priv->data.signalid[SIGNALID_DEFINITION_EDITED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.cell_renderer[CELLRENDERER_DEFINITION]),
          "edited",
          G_CALLBACK (lgw_vocabularywordview_definition_edited_cb),
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


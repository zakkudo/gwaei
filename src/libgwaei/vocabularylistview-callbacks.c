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
    lgw_vocabularylistview_sync_editable (self);

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

    lgw_vocabularylistview_delete_selected (self);
}


void
lgw_vocabularylistview_copy_selected_activated_cb (GSimpleAction *action,
                                                   GVariant      *parameter,
                                                   gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (data));

    //Declarations
    LgwVocabularyListView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (data);

    lgw_vocabularylistview_copy_selected (self);
}


void
lgw_vocabularylistview_cut_selected_activated_cb (GSimpleAction *action,
                                                  GVariant      *parameter,
                                                  gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (data));

    //Declarations
    LgwVocabularyListView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (data);

    lgw_vocabularylistview_cut_selected (self);
}


void
lgw_vocabularylistview_paste_activated_cb (GSimpleAction *action,
                                           GVariant      *parameter,
                                           gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (data));

    //Declarations
    LgwVocabularyListView *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTVIEW (data);

    lgw_vocabularylistview_paste (self);
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
    gchar *uri = NULL;

    //Initializations
    vocabulary_list_store = lgw_vocabularylistview_get_liststore (self);
    if (vocabulary_list_store == NULL) goto errored;
    tree_model = GTK_TREE_MODEL (vocabulary_list_store);
    tree_path = gtk_tree_path_new_from_string (path_string);
    if (tree_path == NULL) goto errored;
    vocabulary_word_store = lgw_vocabularyliststore_get_wordstore (vocabulary_list_store, tree_path);
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    uri = lw_vocabulary_build_uri (new_text);
    if (uri == NULL) goto errored;

    lgw_vocabularyliststore_set (vocabulary_list_store, tree_path, LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME, new_text, -1);

errored:

    if (uri != NULL) g_free (uri); uri = NULL;
    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
}


void
lgw_vocabularylistview_drag_motion_cb (LgwVocabularyListView *self,
                                      GdkDragContext         *drag_context,
                                      gint                    x,
                                      gint                    y,
                                      guint                   time,
                                      GtkTreeView            *inner_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
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
      if (LGW_IS_VOCABULARYLISTSTORE (tree_model))
      {
        gtk_drag_highlight (GTK_WIDGET (inner_tree_view));
        if (drop_position == GTK_TREE_VIEW_DROP_INTO_OR_BEFORE)
          gtk_tree_view_set_drag_dest_row (inner_tree_view, dest_path, GTK_TREE_VIEW_DROP_BEFORE);
        else if (drop_position == GTK_TREE_VIEW_DROP_INTO_OR_AFTER)
          gtk_tree_view_set_drag_dest_row (inner_tree_view, dest_path, GTK_TREE_VIEW_DROP_AFTER);
      }
      if (LGW_IS_VOCABULARYWORDSTORE (tree_model))
      {
        gtk_drag_highlight (GTK_WIDGET (inner_tree_view));
        if (drop_position == GTK_TREE_VIEW_DROP_BEFORE)
          gtk_tree_view_set_drag_dest_row (inner_tree_view, dest_path, GTK_TREE_VIEW_DROP_INTO_OR_BEFORE);
        else if (drop_position == GTK_TREE_VIEW_DROP_AFTER)
          gtk_tree_view_set_drag_dest_row (inner_tree_view, dest_path, GTK_TREE_VIEW_DROP_INTO_OR_AFTER);
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
_move_from_liststore (LgwVocabularyListView *self,
                      GtkTreePath           *destination_tree_path,
                      GtkTreeView           *source_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));
    g_return_if_fail (GTK_IS_TREE_VIEW (source_tree_view));

    //Declarations
    LgwVocabularyListStore *destination_list_store = NULL;
    GtkTreeModel *source_tree_model = NULL;
    GList *wordstores = NULL;
    GList *copied_wordstores = NULL;
    GList *inserted_tree_paths = NULL;
    gboolean success = FALSE;
    
    //Initializations
    destination_list_store = lgw_vocabularylistview_get_liststore (self);
    if (destination_list_store == NULL) goto errored;
    source_tree_model = gtk_tree_view_get_model (source_tree_view);
    if (!LGW_IS_VOCABULARYLISTSTORE (source_tree_model)) goto errored;
    wordstores = lgw_treeview_get_selected_wordstores (source_tree_view);
    if (wordstores == NULL) goto errored;
    inserted_tree_paths = lgw_vocabularyliststore_insert (destination_list_store, destination_tree_path, wordstores);
    if (inserted_tree_paths == NULL) goto errored;

    lgw_vocabularylistview_select (self, inserted_tree_paths);

    success = TRUE;

errored:

    g_list_free_full (inserted_tree_paths, (GDestroyNotify) gtk_tree_path_free); inserted_tree_paths = NULL;
    g_list_free (wordstores); wordstores = NULL; 

    return success;
}


static gboolean
_copy_from_liststore (LgwVocabularyListView *self,
                      GtkTreePath           *destination_tree_path,
                      GtkTreeView           *source_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));
    g_return_if_fail (GTK_IS_TREE_VIEW (source_tree_view));

    //Declarations
    LgwVocabularyListStore *destination_list_store = NULL;
    GtkTreeModel *source_tree_model = NULL;
    GList *wordstores = NULL;
    GList *copied_wordstores = NULL;
    GList *inserted_tree_paths = NULL;
    gboolean success = FALSE;
    GList *original_tree_paths = NULL;
    
    //Initializations
    destination_list_store = lgw_vocabularylistview_get_liststore (self);
    if (destination_list_store == NULL) goto errored;
    source_tree_model = gtk_tree_view_get_model (source_tree_view);
    if (!LGW_IS_VOCABULARYLISTSTORE (source_tree_model)) goto errored;
    wordstores = lgw_treeview_get_selected_wordstores (source_tree_view);
    if (wordstores == NULL) goto errored;

    {
      GList *link = NULL;
      for (link = wordstores; link != NULL; link = link->next)
      {
        LgwVocabularyWordStore *w = LGW_VOCABULARYWORDSTORE (link->data);
        if (w != NULL)
        {
          LgwVocabularyWordStore *copy = lgw_vocabularywordstore_copy (w);
          if (copy != NULL)
          {
            copied_wordstores = g_list_prepend (copied_wordstores, copy);
            copy = NULL;
          }
        }
      }
      copied_wordstores = g_list_reverse (copied_wordstores);
    }
    if (copied_wordstores == NULL) goto errored;

    inserted_tree_paths = lgw_vocabularyliststore_insert (destination_list_store, destination_tree_path, copied_wordstores);
    if (inserted_tree_paths == NULL) goto errored;

    lgw_vocabularylistview_select (self, inserted_tree_paths);

    success = TRUE;

errored:

    g_list_free_full (inserted_tree_paths, (GDestroyNotify) gtk_tree_path_free); inserted_tree_paths = NULL;
    g_list_free (wordstores); wordstores = NULL; 
    g_list_free (copied_wordstores); copied_wordstores = NULL;

    return success;
}


static gboolean
_move_from_wordstore (LgwVocabularyListView *self,
                      GtkTreePath           *destination_tree_path,
                      GtkTreeView           *source_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));
    g_return_if_fail (GTK_IS_TREE_VIEW (source_tree_view));

    //Declarations
    LgwVocabularyListStore *destination_list_store = NULL;
    LgwVocabularyWordStore *destination_word_store = NULL;
    GtkTreeModel *source_tree_model = NULL;
    LgwVocabularyWordStore *source_word_store = NULL;
    GList *words = NULL;
    GList *copied_words = NULL;
    GList *inserted_tree_paths = NULL;
    gboolean success = FALSE;

    //Initializations
    destination_list_store = lgw_vocabularylistview_get_liststore (self);
    if (destination_list_store == NULL) goto errored;
    destination_word_store = lgw_vocabularyliststore_get_wordstore (destination_list_store, destination_tree_path);
    if (destination_word_store == NULL) goto errored;
    source_tree_model = gtk_tree_view_get_model (source_tree_view);
    if (!LGW_IS_VOCABULARYWORDSTORE (source_tree_model)) goto errored;
    source_word_store = LGW_VOCABULARYWORDSTORE (source_tree_model);
    if (source_word_store == NULL) goto errored;
    words = lgw_treeview_get_selected_words (source_tree_view);
    if (words == NULL) goto errored;
    copied_words = g_list_copy_deep (words, (GCopyFunc) lw_word_copy, NULL);
    inserted_tree_paths = lgw_vocabularywordstore_insert (destination_word_store, NULL, copied_words);
    if (inserted_tree_paths = NULL) goto errored;

    success = TRUE;

    lw_vocabulary_remove_words (LW_VOCABULARY (source_word_store), words);

errored:

    g_list_free_full (inserted_tree_paths, (GDestroyNotify) gtk_tree_path_free); inserted_tree_paths = NULL;
    g_list_free (words); words = NULL;
    g_list_free (copied_words); copied_words = NULL;

    return success;
}


static gboolean
_copy_from_wordstore (LgwVocabularyListView *self,
                      GtkTreePath           *destination_tree_path,
                      GtkTreeView           *source_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));
    g_return_if_fail (GTK_IS_TREE_VIEW (source_tree_view));

    //Declarations
    LgwVocabularyListStore *destination_list_store = NULL;
    LgwVocabularyWordStore *destination_word_store = NULL;
    GtkTreeModel *source_tree_model = NULL;
    LgwVocabularyWordStore *source_word_store = NULL;
    GList *words = NULL;
    GList *copied_words = NULL;
    GList *inserted_tree_paths = NULL;
    gboolean success = FALSE;

    //Initializations
    destination_list_store = lgw_vocabularylistview_get_liststore (self);
    if (destination_list_store == NULL) goto errored;
    destination_word_store = lgw_vocabularyliststore_get_wordstore (destination_list_store, destination_tree_path);
    if (destination_word_store == NULL) goto errored;
    source_tree_model = gtk_tree_view_get_model (source_tree_view);
    if (!LGW_IS_VOCABULARYWORDSTORE (source_tree_model)) goto errored;
    source_word_store = LGW_VOCABULARYWORDSTORE (source_tree_model);
    if (source_word_store == NULL) goto errored;
    words = lgw_treeview_get_selected_words (source_tree_view);
    if (words == NULL) goto errored;
    copied_words = g_list_copy_deep (words, (GCopyFunc) lw_word_copy, NULL);
    inserted_tree_paths = lgw_vocabularywordstore_insert (destination_word_store, NULL, copied_words);
    if (inserted_tree_paths = NULL) goto errored;

    success = TRUE;

errored:

    g_list_free_full (inserted_tree_paths, (GDestroyNotify) gtk_tree_path_free); inserted_tree_paths = NULL;
    g_list_free (words); words = NULL;
    g_list_free (copied_words); copied_words = NULL;

    return success;
}


gboolean
lgw_vocabularylistview_drag_drop_cb (LgwVocabularyListView *self,
                                     GdkDragContext        *drag_context,
                                     gint                   x,
                                     gint                   y,
                                     guint                  time,
                                     gpointer              *inner_text_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    GtkTreePath *tree_path = NULL;
    gint success = FALSE;
    GtkWidget *source_widget = NULL;
    GtkTreeView *source_tree_view = NULL;
    GtkTreeModel *source_tree_model = NULL;
    GdkDragAction action = 0;

    //Initializations
    priv = self->priv;
    tree_path = lgw_vocabularylistview_get_tree_path (self, drag_context, x, y);
    source_widget = gtk_drag_get_source_widget (drag_context);
    if (!GTK_IS_TREE_VIEW (source_widget)) goto errored;
    source_tree_view = GTK_TREE_VIEW (source_widget);
    source_tree_model = gtk_tree_view_get_model (source_tree_view);
    if (source_tree_model == NULL) goto errored;
    action = priv->data.suggested_action;
    
    if (LGW_IS_VOCABULARYLISTSTORE (source_tree_model))
    {
      if (action == GDK_ACTION_MOVE)
      {
        success = _move_from_liststore (self, tree_path, source_tree_view);
      }
      else if (action == GDK_ACTION_COPY)
      {
        success = _copy_from_liststore (self, tree_path, source_tree_view);
      }
    }
    else if (LGW_IS_VOCABULARYWORDSTORE (source_tree_model))
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

    gtk_drag_finish (drag_context, success, FALSE, time);

    return TRUE;
}


void
lgw_vocabularylistview_drag_data_received_cb (LgwVocabularyListView *self,
                                              GdkDragContext        *drag_context,
                                              gint                   x,
                                              gint                   y,
                                              GtkSelectionData      *selection_data,
                                              guint                  info,
                                              guint                  time,
                                              GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    GtkWidget *source_widget = NULL;
    GtkTreeModel *tree_model = NULL;
    GdkModifierType mask = 0;;

    //Initializations
    priv = self->priv;
    source_widget = gtk_drag_get_source_widget (drag_context);
    if (!GTK_IS_TREE_VIEW (source_widget)) goto errored;
    tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (source_widget));
    gdk_window_get_device_position (gtk_widget_get_window (GTK_WIDGET (inner_tree_view)), gdk_drag_context_get_device (drag_context), NULL, NULL, &mask);

    if (LGW_IS_VOCABULARYLISTSTORE (tree_model))
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
    else if (LGW_IS_VOCABULARYWORDSTORE (tree_model))
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
lgw_vocabularylistview_drag_leave_cb (LgwVocabularyListView *self,
                                      GdkDragContext        *drag_context,
                                      guint                  time,
                                      GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    gtk_drag_unhighlight (GTK_WIDGET (inner_tree_view));
}


void
lgw_vocabularylistview_filename_column_clicked_cb (LgwVocabularyListView *self,
                                                   GtkTreeViewColumn     *tree_view_column)
{
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (self));
    g_return_if_fail (GTK_IS_TREE_VIEW_COLUMN (tree_view_column));

    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    GtkTreeSortable *tree_sortable = NULL;
    gint sort_column_id = 0;
    GtkSortType order = 0;
    gboolean default_column = TRUE;

    //Initializations
    vocabulary_list_store = lgw_vocabularylistview_get_liststore (self);
    tree_sortable = GTK_TREE_SORTABLE (vocabulary_list_store);
    default_column = gtk_tree_sortable_get_sort_column_id (tree_sortable, &sort_column_id, &order);

    if (sort_column_id == TREEVIEWCOLUMN_NAME && order == GTK_SORT_ASCENDING)
    {
      sort_column_id = TREEVIEWCOLUMN_NAME;
      order = GTK_SORT_DESCENDING;
      gtk_tree_view_column_set_sort_indicator (tree_view_column, TRUE);
      gtk_tree_view_column_set_sort_order (tree_view_column, order);
    }
    else if (sort_column_id == TREEVIEWCOLUMN_NAME && order == GTK_SORT_DESCENDING)
    {
      sort_column_id = GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID;
      order = GTK_SORT_ASCENDING;
      gtk_tree_view_column_set_sort_indicator (tree_view_column, FALSE);
      gtk_tree_view_column_set_sort_order (tree_view_column, order);
    }
    else
    {
      sort_column_id = TREEVIEWCOLUMN_NAME;
      order = GTK_SORT_ASCENDING;
      gtk_tree_view_column_set_sort_indicator (tree_view_column, TRUE);
      gtk_tree_view_column_set_sort_order (tree_view_column, order);
    }

    gtk_tree_sortable_set_sort_column_id (tree_sortable, sort_column_id, order);
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

    if (priv->data.signalid[SIGNALID_DRAG_MOTION] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_MOTION] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-motion",
          G_CALLBACK (lgw_vocabularylistview_drag_motion_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DRAG_DROP] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_DROP] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-drop",
          G_CALLBACK (lgw_vocabularylistview_drag_drop_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DRAG_DATA_RECEIVED] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_DATA_RECEIVED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-data-received",
          G_CALLBACK (lgw_vocabularylistview_drag_data_received_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_DRAG_LEAVE] == 0)
    {
      priv->data.signalid[SIGNALID_DRAG_LEAVE] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "drag-leave",
          G_CALLBACK (lgw_vocabularylistview_drag_leave_cb),
          self
      );
    }

    if (priv->data.signalid[SIGNALID_FILENAME_COLUMN_CLICKED] == 0)
    {
      priv->data.signalid[SIGNALID_FILENAME_COLUMN_CLICKED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view_column[TREEVIEWCOLUMN_NAME]),
          "clicked",
          G_CALLBACK (lgw_vocabularylistview_filename_column_clicked_cb),
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

    if (priv->data.signalid[SIGNALID_FILENAME_COLUMN_CLICKED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.tree_view_column[TREEVIEWCOLUMN_NAME]),
        priv->data.signalid[SIGNALID_FILENAME_COLUMN_CLICKED]
      );
      priv->data.signalid[SIGNALID_FILENAME_COLUMN_CLICKED] = 0;
    }
}



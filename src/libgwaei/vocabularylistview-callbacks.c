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
//! @file vocabularylistview.c
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
lgw_vocabularylistview_connect_signals (LgwVocabularyListView *vocabulary_list_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (vocabulary_list_view));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    priv = vocabulary_list_view->priv;

    if (priv->data.signalid[SIGNALID_SELECTION_CHANGED] == 0)
    {
        priv->data.signalid[SIGNALID_SELECTION_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->data.tree_selection),
          "changed",
          G_CALLBACK (lgw_vocabularylistview_selection_changed_cb),
          vocabulary_list_view
        );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_IN_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "focus-in-event",
          G_CALLBACK (lgw_vocabularylistview_focus_in_event_cb),
          vocabulary_list_view
      );
    }

    if (priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] == 0)
    {
      priv->data.signalid[SIGNALID_FOCUS_OUT_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.tree_view),
          "focus-out-event",
          G_CALLBACK (lgw_vocabularylistview_focus_out_event_cb),
          vocabulary_list_view
      );
    }
}


void
lgw_vocabularylistview_disconnect_signals (LgwVocabularyListView *vocabulary_list_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (vocabulary_list_view));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;

    //Initializations
    priv = vocabulary_list_view->priv;

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
}


void
lgw_vocabularylistview_selection_changed_cb (LgwVocabularyListView *vocabulary_list_view,
                                             GtkTreeSelection      *tree_selection)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (vocabulary_list_view));

    //Declarations
    LgwVocabularyListViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordView *vocabulary_word_view = NULL;

    //Initializations
    priv = vocabulary_list_view->priv;
    if (priv == NULL) goto errored;
    vocabulary_word_view = lgw_vocabularylistview_get_wordview (vocabulary_list_view);
    if (vocabulary_word_view == NULL) goto errored;
    vocabulary_word_store = lgw_vocabularylistview_get_selected_wordstore (vocabulary_list_view);

    if (vocabulary_word_store != NULL)
    {
      lw_vocabulary_load (LW_VOCABULARY (vocabulary_word_store), NULL);
    }

    lgw_vocabularywordview_set_wordstore (vocabulary_word_view, vocabulary_word_store);

errored:

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
    LgwVocabularyListView *vocabulary_list_view = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    gchar *filename = NULL;

    //Initializations
    vocabulary_list_view = LGW_VOCABULARYLISTVIEW (data);
    filename = lgw_vocabularywordstore_generate_filename ();
    if (filename == NULL) goto errored;

    vocabulary_word_store = lgw_vocabularywordstore_new (filename);

errored:

    if (filename != NULL) g_free (filename); filename = NULL;

    //TODO append
}

void
lgw_vocabularylistview_remove_selected_activated_cb (GSimpleAction *action,
                                                     GVariant      *parameter,
                                                     gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYLISTVIEW (data));

    //Declarations
    LgwVocabularyListView *vocabulary_list_view = NULL;
    LgwVocabularyListViewPrivate *priv = NULL;
    GtkTreeModel *tree_model = NULL;
    GList *rowlist = NULL;

    //Initializations
    vocabulary_list_view = LGW_VOCABULARYLISTVIEW (data);
    priv = vocabulary_list_view->priv;
    tree_model = GTK_TREE_MODEL (priv->data.vocabulary_list_store);
    rowlist = gtk_tree_selection_get_selected_rows (priv->data.tree_selection, &tree_model);

errored:

    if (rowlist != NULL) g_list_free_full (rowlist, (GDestroyNotify) gtk_tree_path_free); rowlist = NULL;
}


gboolean
lgw_vocabularylistview_focus_in_event_cb (LgwVocabularyListView *vocabulary_list_view,
                                          GdkEvent              *event,
                                          GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (vocabulary_list_view), FALSE);
    g_return_val_if_fail (GTK_IS_TREE_VIEW (inner_tree_view), FALSE);

    //Declarations
    LgwActionable *actionable = NULL;

    //Initializations
    actionable = LGW_ACTIONABLE (vocabulary_list_view);

    lgw_actionable_sync_actions (actionable);

    return FALSE;
}


gboolean
lgw_vocabularylistview_focus_out_event_cb (LgwVocabularyListView *vocabulary_list_view,
                                           GdkEvent              *event,
                                           GtkTreeView           *inner_tree_view)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTVIEW (vocabulary_list_view), FALSE);
    g_return_val_if_fail (GTK_IS_TREE_VIEW (inner_tree_view), FALSE);

    //Declarations
    LgwActionable *actionable = NULL;

    //Initializations
    actionable = LGW_ACTIONABLE (vocabulary_list_view);

    lgw_actionable_sync_actions (actionable);

    return FALSE;
}


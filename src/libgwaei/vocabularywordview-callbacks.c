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

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularywordview-private.h>


void
lgw_vocabularywordview_connect_signals (LgwVocabularyWordView *vocabulary_word_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (vocabulary_word_view));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Initializations
    priv = vocabulary_word_view->priv;

    if (priv->data.signalid[SIGNALID_SELECTION_CHANGED] == 0)
    {
        priv->data.signalid[SIGNALID_SELECTION_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->data.tree_selection),
          "changed",
          G_CALLBACK (lgw_vocabularywordview_selection_changed_cb),
          vocabulary_word_view
        );
    }
}


void
lgw_vocabularywordview_disconnect_signals (LgwVocabularyWordView *vocabulary_word_view)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (vocabulary_word_view));

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;

    //Initializations
    priv = vocabulary_word_view->priv;

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
}


void
lgw_vocabularywordview_selection_changed_cb (LgwVocabularyWordView *vocabulary_word_view,
                                             GtkTreeSelection      *tree_selection)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (vocabulary_word_view));

    printf("BREAK lgw_vocabularywordview_selection_changed_cb\n");

    //Declarations
    LgwVocabularyWordViewPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;

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
    LgwVocabularyWordView *vocabulary_word_view = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    gchar *filename = NULL;

    //Initializations
    vocabulary_word_view = LGW_VOCABULARYWORDVIEW (data);
    filename = lgw_vocabularywordstore_generate_filename ();
    if (filename == NULL) goto errored;

    vocabulary_word_store = lgw_vocabularywordstore_new (filename);

errored:

    if (filename != NULL) g_free (filename); filename = NULL;

    //TODO append
}

void
lgw_vocabularywordview_remove_selected_activated_cb (GSimpleAction *action,
                                                     GVariant      *parameter,
                                                     gpointer       data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDVIEW (data));

    //Declarations
    LgwVocabularyWordView *vocabulary_word_view = NULL;
    LgwVocabularyWordViewPrivate *priv = NULL;
    GtkTreeModel *tree_model = NULL;
    GList *rowlist = NULL;

    //Initializations
    vocabulary_word_view = LGW_VOCABULARYWORDVIEW (data);
    priv = vocabulary_word_view->priv;
    tree_model = GTK_TREE_MODEL (priv->data.vocabulary_word_store);
    rowlist = gtk_tree_selection_get_selected_rows (priv->data.tree_selection, &tree_model);

errored:

    if (rowlist != NULL) g_list_free_full (rowlist, (GDestroyNotify) gtk_tree_path_free); rowlist = NULL;
}


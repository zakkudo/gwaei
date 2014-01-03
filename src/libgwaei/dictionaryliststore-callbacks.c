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
//! @file dictionarylist-model.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/dictionaryliststore-private.h>


void
lgw_dictionaryliststore_changed_cb (LgwDictionaryListStore *dictionary_list_store,
                               gint               position,
                               gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list_store != NULL);

    //Declarations
    LgwDictionaryListStorePrivate *priv = NULL;
    GtkTreeIter iter;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list_store->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;
    lgw_dictionaryliststore_initialize_tree_iter (dictionary_list_store, &iter, position);
    if (!lgw_dictionaryliststore_tree_iter_is_valid (dictionary_list_store, &iter)) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list_store),
      "row-changed",
      path,
      &iter,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}


void
lgw_dictionaryliststore_inserted_cb (LgwDictionaryListStore *dictionary_list_store,
                                gint               position,
                                gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list_store != NULL);

    //Declarations
    LgwDictionaryListStorePrivate *priv = NULL;
    GtkTreeIter iter;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list_store->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;
    lgw_dictionaryliststore_initialize_tree_iter (dictionary_list_store, &iter, position);
    if (!lgw_dictionaryliststore_tree_iter_is_valid (dictionary_list_store, &iter)) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list_store),
      "row-inserted",
      path,
      &iter,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}

void
lgw_dictionaryliststore_deleted_cb (LgwDictionaryListStore *dictionary_list_store,
                               gint               position,
                               gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list_store != NULL);

    //Declarations
    LgwDictionaryListStorePrivate *priv = NULL;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list_store->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list_store),
      "row-deleted",
      path,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}

void
lgw_dictionaryliststore_reordered_cb (LgwDictionaryListStore *dictionary_list_store,
                                 gint              *new_order,
                                 gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list_store != NULL);

    //Declarations
    LgwDictionaryListStorePrivate *priv = NULL;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list_store->priv;
    path = gtk_tree_path_new_from_indices (-1);
    if (path == NULL) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list_store),
      "rows-reordered",
      path,
      NULL,
      new_order,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}


void
lgw_dictionaryliststore_connect_signals (LgwDictionaryListStore *dictionary_list_store)
{
    //Sanity checks
    g_return_if_fail (dictionary_list_store != NULL);

    //Declarations
    LgwDictionaryListStorePrivate *priv = NULL;

    //Initializations
    priv = dictionary_list_store->priv;

    if (priv->data.signalid[SIGNALID_CHANGED] == 0)
    {
      priv->data.signalid[SIGNALID_CHANGED] = g_signal_connect (
        G_OBJECT (dictionary_list_store),
        "internal-row-changed",
        G_CALLBACK (lgw_dictionaryliststore_changed_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_INSERTED] == 0)
    {
      priv->data.signalid[SIGNALID_INSERTED] = g_signal_connect (
        G_OBJECT (dictionary_list_store),
        "internal-row-inserted",
        G_CALLBACK (lgw_dictionaryliststore_inserted_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_DELETED] == 0)
    {
      priv->data.signalid[SIGNALID_DELETED] = g_signal_connect (
        G_OBJECT (dictionary_list_store),
        "internal-row-deleted",
        G_CALLBACK (lgw_dictionaryliststore_deleted_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_REORDERED] == 0)
    {
      priv->data.signalid[SIGNALID_REORDERED] = g_signal_connect (
        G_OBJECT (dictionary_list_store),
        "internal-rows-reordered",
        G_CALLBACK (lgw_dictionaryliststore_reordered_cb),
        NULL
      );
    }
}


void
lgw_dictionaryliststore_disconnect_signals (LgwDictionaryListStore *dictionary_list_store)
{
    //Sanity checks
    g_return_if_fail (dictionary_list_store != NULL);

    //Declarations
    LgwDictionaryListStorePrivate *priv = NULL;
    gint i = 0;

    //Initializations
    priv = dictionary_list_store->priv;

    for (i = 0; i < TOTAL_SIGNALIDS; i++)
    {
      if (priv->data.signalid[i] != 0)
      {
        g_signal_handler_disconnect (G_OBJECT (dictionary_list_store), priv->data.signalid[i]);
        priv->data.signalid[i] = 0;
      }
    }
}











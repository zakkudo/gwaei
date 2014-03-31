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
//! @file vocabularywordstore.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib-object.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularywordstore-private.h>


void
lgw_vocabularywordstore_changed_cb (LgwVocabularyWordStore *self,
                                    gint                    position,
                                    gpointer                data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    GtkTreeModel *tree_model = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    GtkTreeIter iter;
    GtkTreePath *path = NULL;

    //Initializations
    tree_model = GTK_TREE_MODEL (self);
    priv = self->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;
    gtk_tree_model_get_iter (tree_model, &iter, path);

    g_signal_emit_by_name (G_OBJECT (self),
      "row-changed",
      path,
      &iter,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}


void
lgw_vocabularywordstore_inserted_cb (LgwVocabularyWordStore *self,
                                     gint                    position,
                                     gpointer                data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    GtkTreeModel *tree_model = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    GtkTreeIter iter;
    GtkTreePath *path = NULL;

    //Initializations
    tree_model = GTK_TREE_MODEL (self);
    priv = self->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;
    gtk_tree_model_get_iter (tree_model, &iter, path);

    g_signal_emit_by_name (G_OBJECT (self),
      "row-inserted",
      path,
      &iter,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}

void
lgw_vocabularywordstore_deleted_cb (LgwVocabularyWordStore *self,
                                    gint               position,
                                    gpointer           data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LgwVocabularyWordStorePrivate *priv = NULL;
    GtkTreePath *path = NULL;

    //Initializations
    priv = self->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;

    g_signal_emit_by_name (G_OBJECT (self),
      "row-deleted",
      path,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}

void
lgw_vocabularywordstore_reordered_cb (LgwVocabularyWordStore *self,
                                      gint                   *new_order,
                                      gpointer                data)
{
printf("BREAK lgw_vocabularywordstore_reordered_cb\n");
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LgwVocabularyWordStorePrivate *priv = NULL;
    GtkTreePath *path = NULL;

    //Initializations
    priv = self->priv;
    path = gtk_tree_path_new ();
    if (path == NULL) goto errored;

    g_signal_emit_by_name (G_OBJECT (self),
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
lgw_vocabularywordstore_connect_signals (LgwVocabularyWordStore *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LgwVocabularyWordStorePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_CHANGED] == 0)
    {
      priv->data.signalid[SIGNALID_CHANGED] = g_signal_connect (
        G_OBJECT (self),
        "internal-row-changed",
        G_CALLBACK (lgw_vocabularywordstore_changed_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_INSERTED] == 0)
    {
      priv->data.signalid[SIGNALID_INSERTED] = g_signal_connect (
        G_OBJECT (self),
        "internal-row-inserted",
        G_CALLBACK (lgw_vocabularywordstore_inserted_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_DELETED] == 0)
    {
      priv->data.signalid[SIGNALID_DELETED] = g_signal_connect (
        G_OBJECT (self),
        "internal-row-deleted",
        G_CALLBACK (lgw_vocabularywordstore_deleted_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_REORDERED] == 0)
    {
      priv->data.signalid[SIGNALID_REORDERED] = g_signal_connect (
        G_OBJECT (self),
        "internal-rows-reordered",
        G_CALLBACK (lgw_vocabularywordstore_reordered_cb),
        NULL
      );
    }
}


void
lgw_vocabularywordstore_disconnect_signals (LgwVocabularyWordStore *self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LgwVocabularyWordStorePrivate *priv = NULL;
    gint i = 0;

    //Initializations
    priv = self->priv;

    for (i = 0; i < TOTAL_SIGNALIDS; i++)
    {
      if (priv->data.signalid[i] != 0)
      {
        g_signal_handler_disconnect (G_OBJECT (self), priv->data.signalid[i]);
        priv->data.signalid[i] = 0;
      }
    }
}











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
//! @file vocabularyliststore.c
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


static gint stamp = 0;
static gint valid_stamp = 0;

void
lgw_vocabularyliststore_init_interface (GtkTreeModelIface *iface)
{
    iface->get_flags = lgw_vocabularyliststore_get_flags;
    iface->get_n_columns = lgw_vocabularyliststore_get_n_columns;
    iface->get_column_type = lgw_vocabularyliststore_get_column_type;
    iface->get_iter = lgw_vocabularyliststore_get_iter;
    iface->get_path = lgw_vocabularyliststore_get_path;
    iface->get_value = lgw_vocabularyliststore_get_value;
    iface->iter_next = lgw_vocabularyliststore_iter_next;
    iface->iter_previous = lgw_vocabularyliststore_iter_previous;
    iface->iter_children = lgw_vocabularyliststore_iter_children;
    iface->iter_has_child = lgw_vocabularyliststore_iter_has_child;
    iface->iter_n_children = lgw_vocabularyliststore_iter_n_children;
    iface->iter_nth_child = lgw_vocabularyliststore_iter_nth_child;
    iface->iter_parent = lgw_vocabularyliststore_iter_parent;
}



void
lgw_vocabularyliststore_initialize_tree_iter (LgwVocabularyListStore *vocabulary_list_store,
                                              GtkTreeIter            *iter,
                                              gint                    index_)
{
    //Sanity checks
    g_return_val_if_fail (vocabulary_list_store != NULL, 0);

    //Declarations
    LgwVocabularyListStoreClass *klass = NULL;

    //Initializations
    iter->stamp = stamp;
    iter->user_data = vocabulary_list_store;
    iter->user_data2 = GINT_TO_POINTER (index_);

    stamp++;
}


void
lgw_vocabularyliststore_invalidate_tree_iter (GtkTreeIter *iter)
{
    //Sanity checks
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(GtkTreeIter));
    iter->stamp = -1;
}


gboolean
lgw_vocabularyliststore_tree_iter_is_valid (LgwVocabularyListStore *vocabulary_list_store,
                                            GtkTreeIter            *iter)
{
    //Sanity checks
    g_return_val_if_fail (vocabulary_list_store != NULL, FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gint index = 0;

    //Initializations
    priv = vocabulary_list_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2);

    return (iter->stamp > -1 && iter->stamp > valid_stamp && index >= 0 && index < priv->data.length);
}


void
lgw_vocabularyliststore_invalidate_old_timestamps ()
{
  valid_stamp = stamp - 1;
}


GtkTreeModelFlags
lgw_vocabularyliststore_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_LIST_ONLY;
}


gint
lgw_vocabularyliststore_get_n_columns (GtkTreeModel *tree_model)
{
    return TOTAL_LGW_VOCABULARYLISTSTORE_COLUMNS;
}


GType
lgw_vocabularyliststore_get_column_type (GtkTreeModel *tree_model,
                                         gint          index_)
{
    GType type = G_TYPE_INVALID;

    switch (index_)
    {
      case LGW_VOCABULARYLISTSTORE_COLUMN_NAME:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_HAS_CHANGES:
        type = G_TYPE_BOOLEAN;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT:
        type = G_TYPE_OBJECT;
        break;
    }

    return type;
}


gboolean
lgw_vocabularyliststore_get_iter (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter,
                                  GtkTreePath  *path)
{
    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gboolean is_valid = FALSE;
    gint depth = 0;
    gint index = 0;
    gint* indicies = NULL;

    if (tree_model == NULL) goto errored;
    if (path == NULL) goto errored;
    if (iter == NULL) goto errored;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = vocabulary_list_store->priv;
    indicies = gtk_tree_path_get_indices_with_depth (path, &depth);
    if (indicies != NULL)
    {
      index = indicies[0];
    }
    is_valid = (depth == 1 && index >= 0 && index < priv->data.length);

    if (iter != NULL && is_valid)
    {
      lgw_vocabularyliststore_initialize_tree_iter (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter, index);
    }

errored:

    if (!lgw_vocabularyliststore_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter))
    {
      lgw_vocabularyliststore_invalidate_tree_iter (iter);
    }

    return is_valid;
}


GtkTreePath*
lgw_vocabularyliststore_get_path (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (tree_model != NULL, NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    //Declarations
    GtkTreePath *path = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);

    if (lgw_vocabularyliststore_tree_iter_is_valid (vocabulary_list_store, iter))
    {
      path = gtk_tree_path_new_from_indices (GPOINTER_TO_INT (iter->user_data2), -1);
    }

    return path;
}


void
lgw_vocabularyliststore_get_value (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter,
                                   gint          column,
                                   GValue       *value)
{
    //Sanity checks
    g_return_if_fail (tree_model != NULL);
    g_return_if_fail (iter != NULL);
    g_return_if_fail (value != NULL);

    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    GType type = G_TYPE_INVALID;
    gint index = 0;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;

 
    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = vocabulary_list_store->priv;
    type = lgw_vocabularyliststore_get_column_type (tree_model, column);
    index = GPOINTER_TO_INT (iter->user_data2);
    vocabulary_word_store = g_list_nth_data (priv->data.list, index);
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    g_value_init (value, type);

    if (vocabulary_word_store == NULL) goto errored;

    switch (column)
    {
      case LGW_VOCABULARYLISTSTORE_COLUMN_NAME: //G_TYPE_STRING
        g_value_set_string (value, lw_vocabulary_get_filename (vocabulary));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME: //G_TYPE_STRING
        g_value_set_string (value, lw_vocabulary_get_filename (vocabulary));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_HAS_CHANGES: //G_TYPE_BOOLEAN
        g_value_set_boolean (value, lw_vocabulary_has_changes (vocabulary));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT: //G_TYPE_OBJECT
        g_value_set_object (value, vocabulary_word_store);
        break;
    }

errored:

    if (!lgw_vocabularyliststore_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter))
    {
      lgw_vocabularyliststore_invalidate_tree_iter (iter);
    }

    return;
}


gboolean
lgw_vocabularyliststore_iter_next (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gboolean has_next = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = vocabulary_list_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2) + 1;
    has_next = (index < priv->data.length);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_vocabularyliststore_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter))
    {
      lgw_vocabularyliststore_invalidate_tree_iter (iter);
    }

    return has_next;
}


gboolean
lgw_vocabularyliststore_iter_previous (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv;
    gboolean has_previous = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = vocabulary_list_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    has_previous = (priv->data.length > 0 && index >= 0);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_vocabularyliststore_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter))
    {
      lgw_vocabularyliststore_invalidate_tree_iter (iter);
    }

    return has_previous;
}


gboolean
lgw_vocabularyliststore_iter_children (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter,
                                       GtkTreeIter  *parent)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = vocabulary_list_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    iter->user_data2 = GINT_TO_POINTER (index);

    if (parent == NULL && priv->data.length > 0)
    {
      lgw_vocabularyliststore_initialize_tree_iter (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter, 0);
    }

errored:

    if (!lgw_vocabularyliststore_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter))
    {
      lgw_vocabularyliststore_invalidate_tree_iter (iter);
    }

    return (parent == NULL);
}


gboolean
lgw_vocabularyliststore_iter_has_child (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter)
{
    return FALSE;
}


gint
lgw_vocabularyliststore_iter_n_children (GtkTreeModel *tree_model,
                                         GtkTreeIter  *iter)
{
    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gint total = 0;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    if (vocabulary_list_store == NULL) goto errored;
    priv = vocabulary_list_store->priv;

    if (iter == NULL)
    {
      total = priv->data.length;
    }

errored:

    if (!lgw_vocabularyliststore_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter))
    {
      lgw_vocabularyliststore_invalidate_tree_iter (iter);
    }
    
    return total;
}


gboolean
lgw_vocabularyliststore_iter_nth_child (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter,
                                        GtkTreeIter  *parent,
                                        gint          n)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gint total = 0;
    gboolean exists = FALSE;

    //Initializations
    vocabulary_list_store = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = vocabulary_list_store->priv;
    if (vocabulary_list_store == NULL) goto errored;

    if (parent == NULL)
    {
      total = priv->data.length;
    }
    exists = (vocabulary_list_store != NULL && parent == NULL && total > 0 && n >= 0 && n < total);

errored:

    if (exists)
    {
      lgw_vocabularyliststore_initialize_tree_iter (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter, n);
    }

    if (!lgw_vocabularyliststore_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (vocabulary_list_store), iter))
    {
      lgw_vocabularyliststore_invalidate_tree_iter (iter);
      exists = FALSE;
    }
    
    return exists;
}


gboolean
lgw_vocabularyliststore_iter_parent (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter,
                                GtkTreeIter  *child)
{
    return FALSE;
}


void
lgw_vocabularyliststore_ref_node (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter)
{
    //No-op
}


void
lgw_vocabularyliststore_unref_node (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //No-op
}


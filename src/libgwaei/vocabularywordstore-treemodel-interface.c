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
//! @file vocabularywordstore-treemodel-interface.c
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

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/vocabularywordstore-private.h>


static gint stamp = 0;
static gint valid_stamp = 0;

void
lgw_vocabularywordstore_init_interface (GtkTreeModelIface *iface)
{
    iface->get_flags = lgw_vocabularywordstore_get_flags;
    iface->get_n_columns = lgw_vocabularywordstore_get_n_columns;
    iface->get_column_type = lgw_vocabularywordstore_get_column_type;
    iface->get_iter = lgw_vocabularywordstore_get_iter;
    iface->get_path = lgw_vocabularywordstore_get_path;
    iface->get_value = lgw_vocabularywordstore_get_value;
    iface->iter_next = lgw_vocabularywordstore_iter_next;
    iface->iter_previous = lgw_vocabularywordstore_iter_previous;
    iface->iter_children = lgw_vocabularywordstore_iter_children;
    iface->iter_has_child = lgw_vocabularywordstore_iter_has_child;
    iface->iter_n_children = lgw_vocabularywordstore_iter_n_children;
    iface->iter_nth_child = lgw_vocabularywordstore_iter_nth_child;
    iface->iter_parent = lgw_vocabularywordstore_iter_parent;
}


void
lgw_vocabularywordstore_initialize_tree_iter (LgwVocabularyWordStore *vocabulary_word_store,
                                              GtkTreeIter            *iter,
                                              gint                    index_)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (vocabulary_word_store), 0);

    //Declarations
    LgwVocabularyWordStoreClass *klass = NULL;

    //Initializations
    iter->stamp = stamp;
    iter->user_data = vocabulary_word_store;
    iter->user_data2 = GINT_TO_POINTER (index_);

    stamp++;
}


void
lgw_vocabularywordstore_invalidate_tree_iter (GtkTreeIter *iter)
{
    //Sanity checks
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(GtkTreeIter));
    iter->stamp = -1;
}


gboolean
lgw_vocabularywordstore_tree_iter_is_valid (LgwVocabularyWordStore *vocabulary_word_store,
                                            GtkTreeIter            *iter)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (vocabulary_word_store), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStorePrivate *priv = NULL;
    gint index = 0;

    //Initializations
    priv = vocabulary_word_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2);

    return (iter->stamp > -1 && iter->stamp > valid_stamp && index >= 0 && index < priv->data.length);
}


void
lgw_vocabularywordstore_invalidate_old_timestamps ()
{
  valid_stamp = stamp - 1;
}


GtkTreeModelFlags
lgw_vocabularywordstore_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_LIST_ONLY;
}


gint
lgw_vocabularywordstore_get_n_columns (GtkTreeModel *tree_model)
{
    return TOTAL_LGW_VOCABULARYWORDSTORE_COLUMNS;
}


GType
lgw_vocabularywordstore_get_column_type (GtkTreeModel *tree_model,
                                         gint          index_)
{
    GType type = G_TYPE_INVALID;

    switch (index_)
    {
      case LGW_VOCABULARYWORDSTORE_COLUMN_POSITION:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_KANJI:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_READING:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_DEFINITION:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_CORRECT_GUESSES:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_INCORRECT_GUESSES:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_SCORE:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_LAST_STUDIED:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_WEIGHT:
        type = G_TYPE_INT;
        break;
      default:
        g_assert_not_reached ();
    }

    return type;
}


gboolean
lgw_vocabularywordstore_get_iter (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter,
                                  GtkTreePath  *path)
{
    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    gboolean is_valid = FALSE;
    gint depth = 0;
    gint index = 0;
    gint* indicies = NULL;

    if (tree_model == NULL) goto errored;
    if (path == NULL) goto errored;
    if (iter == NULL) goto errored;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = vocabulary_word_store->priv;
    indicies = gtk_tree_path_get_indices_with_depth (path, &depth);
    if (indicies != NULL)
    {
      index = indicies[0];
    }
    is_valid = (depth == 1 && index >= 0 && index < priv->data.length);

    if (iter != NULL && is_valid)
    {
      lgw_vocabularywordstore_initialize_tree_iter (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter, index);
    }

errored:

    if (!lgw_vocabularywordstore_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter))
    {
      lgw_vocabularywordstore_invalidate_tree_iter (iter);
    }

    return is_valid;
}


GtkTreePath*
lgw_vocabularywordstore_get_path (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (GTK_IS_TREE_MODEL (tree_model), NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    //Declarations
    GtkTreePath *path = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);

    if (lgw_vocabularywordstore_tree_iter_is_valid (vocabulary_word_store, iter))
    {
      path = gtk_tree_path_new_from_indices (GPOINTER_TO_INT (iter->user_data2), -1);
    }

    return path;
}


void
lgw_vocabularywordstore_get_value (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter,
                                   gint          column,
                                   GValue       *value)
{
    //Sanity checks
    g_return_if_fail (GTK_IS_TREE_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (value != NULL);

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    GType type = G_TYPE_INVALID;
    gint index = 0;
    LwVocabulary *vocabulary = NULL;
    LwWord *word = NULL;

 
    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = vocabulary_word_store->priv;
    type = lgw_vocabularywordstore_get_column_type (tree_model, column);
    index = GPOINTER_TO_INT (iter->user_data2);
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    word = lw_vocabulary_get_word_by_index (vocabulary, index);
    g_value_init (value, type);

    if (vocabulary_word_store == NULL) goto errored;
    if (index < 0) goto errored;

    switch (column)
    {
      case LGW_VOCABULARYWORDSTORE_COLUMN_POSITION: //G_TYPE_STRING
        g_value_take_string (value, g_strdup_printf("%d", index + 1));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_KANJI:  //G_TYPE_STRING
        g_value_set_string (value, lw_word_get_kanji (word));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_READING:  //G_TYPE_STRING
        g_value_set_string (value, lw_word_get_reading (word));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_DEFINITION:  //G_TYPE_STRING
        g_value_set_string (value, lw_word_get_definition (word));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_CORRECT_GUESSES:  //G_TYPE_STRING
        g_value_take_string (value, g_strdup_printf ("%d", lw_word_get_correct_guesses (word)));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_INCORRECT_GUESSES:  //G_TYPE_STRING
        g_value_take_string (value, g_strdup_printf ("%d", lw_word_get_incorrect_guesses (word)));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_SCORE:  //G_TYPE_STRING
        g_value_set_string (value, lw_word_get_score_as_string (word));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_LAST_STUDIED:  //G_TYPE_STRING
        g_value_set_string (value, lw_word_get_last_studied_as_string (word));
        break;
      case LGW_VOCABULARYWORDSTORE_COLUMN_WEIGHT:  //G_TYPE_INT
        g_value_set_int (value, lw_word_has_changes (word));
        break;
      default:
        g_assert_not_reached ();
    }

errored:

    if (!lgw_vocabularywordstore_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter))
    {
      lgw_vocabularywordstore_invalidate_tree_iter (iter);
    }

    return;
}


gboolean
lgw_vocabularywordstore_iter_next (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    gboolean has_next = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = vocabulary_word_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2) + 1;
    has_next = (index < priv->data.length);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_vocabularywordstore_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter))
    {
      lgw_vocabularywordstore_invalidate_tree_iter (iter);
    }

    return has_next;
}


gboolean
lgw_vocabularywordstore_iter_previous (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv;
    gboolean has_previous = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = vocabulary_word_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    has_previous = (priv->data.length > 0 && index >= 0);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_vocabularywordstore_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter))
    {
      lgw_vocabularywordstore_invalidate_tree_iter (iter);
    }

    return has_previous;
}


gboolean
lgw_vocabularywordstore_iter_children (GtkTreeModel *tree_model,
                                       GtkTreeIter  *iter,
                                       GtkTreeIter  *parent)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = vocabulary_word_store->priv;
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    iter->user_data2 = GINT_TO_POINTER (index);

    if (parent == NULL && priv->data.length > 0)
    {
      lgw_vocabularywordstore_initialize_tree_iter (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter, 0);
    }

errored:

    if (!lgw_vocabularywordstore_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter))
    {
      lgw_vocabularywordstore_invalidate_tree_iter (iter);
    }

    return (parent == NULL);
}


gboolean
lgw_vocabularywordstore_iter_has_child (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter)
{
    return FALSE;
}


gint
lgw_vocabularywordstore_iter_n_children (GtkTreeModel *tree_model,
                                         GtkTreeIter  *iter)
{
    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    gint total = 0;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    if (vocabulary_word_store == NULL) goto errored;
    priv = vocabulary_word_store->priv;

    if (iter == NULL)
    {
      total = priv->data.length;
    }

errored:

    if (!lgw_vocabularywordstore_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter))
    {
      lgw_vocabularywordstore_invalidate_tree_iter (iter);
    }
    
    return total;
}


gboolean
lgw_vocabularywordstore_iter_nth_child (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter,
                                        GtkTreeIter  *parent,
                                        gint          n)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    gint total = 0;
    gboolean exists = FALSE;

    //Initializations
    vocabulary_word_store = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = vocabulary_word_store->priv;
    if (vocabulary_word_store == NULL) goto errored;

    if (parent == NULL)
    {
      total = priv->data.length;
    }
    exists = (vocabulary_word_store != NULL && parent == NULL && total > 0 && n >= 0 && n < total);

errored:

    if (exists)
    {
      lgw_vocabularywordstore_initialize_tree_iter (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter, n);
    }

    if (!lgw_vocabularywordstore_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (vocabulary_word_store), iter))
    {
      lgw_vocabularywordstore_invalidate_tree_iter (iter);
      exists = FALSE;
    }
    
    return exists;
}


gboolean
lgw_vocabularywordstore_iter_parent (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter,
                                GtkTreeIter  *child)
{
    return FALSE;
}


void
lgw_vocabularywordstore_ref_node (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter)
{
    //No-op
}


void
lgw_vocabularywordstore_unref_node (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //No-op
}


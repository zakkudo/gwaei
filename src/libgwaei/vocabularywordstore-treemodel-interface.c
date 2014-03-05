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


static void
_initialize_tree_iter (LgwVocabularyWordStore *self,
                       GtkTreeIter            *iter,
                       gint                    index_)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (self), 0);

    //Declarations
    LgwVocabularyWordStoreClass *klass = NULL;

    //Initializations
    iter->stamp = stamp;
    iter->user_data = self;
    iter->user_data2 = GINT_TO_POINTER (index_);

    stamp++;
}


static void
_invalidate_tree_iter (GtkTreeIter *iter)
{
    //Sanity checks
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(GtkTreeIter));
    iter->stamp = -1;
}


static gboolean
_tree_iter_is_valid (LgwVocabularyWordStore *self,
                     GtkTreeIter            *iter)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint index = 0;
    gint length = 0;

    //Initializations
    vocabulary = LW_VOCABULARY (self);
    priv = self->priv;
    index = GPOINTER_TO_INT (iter->user_data2);
    length = lw_vocabulary_length (vocabulary);

    return (iter->stamp > -1 && iter->stamp > valid_stamp && index >= 0 && index < length);
}


void
lgw_vocabularywordstore_invalidate_old_timestamps ()
{
  valid_stamp = stamp - 1;
}


static GtkTreeModelFlags
_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_LIST_ONLY | GTK_TREE_MODEL_ITERS_PERSIST;
}


static gint
_get_n_columns (GtkTreeModel *tree_model)
{
    return TOTAL_LGW_VOCABULARYWORDSTORE_COLUMNS;
}


static GType
_get_column_type (GtkTreeModel *tree_model,
                  gint          index_)
{
    GType type = G_TYPE_INVALID;

    switch (index_)
    {
      case LGW_VOCABULARYWORDSTORE_COLUMN_SAVED_POSITION:
        type = G_TYPE_STRING;
        break;
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
      case LGW_VOCABULARYWORDSTORE_COLUMN_WORD:
        type = G_TYPE_POINTER;
        break;
      default:
        g_assert_not_reached ();
    }

    return type;
}


static gboolean
_get_iter (GtkTreeModel *tree_model,
           GtkTreeIter  *iter,
           GtkTreePath  *path)
{
    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint length = 0;
    gboolean is_valid = FALSE;
    gint depth = 0;
    gint index = 0;
    gint* indicies = NULL;

    if (tree_model == NULL) goto errored;
    if (path == NULL) goto errored;
    if (iter == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = self->priv;
    indicies = gtk_tree_path_get_indices_with_depth (path, &depth);
    vocabulary = LW_VOCABULARY (self);
    length = lw_vocabulary_length (vocabulary);

    if (indicies != NULL)
    {
      index = indicies[0];
    }
    is_valid = (depth == 1 && index >= 0 && index < length);

    if (iter != NULL && is_valid)
    {
      _initialize_tree_iter (LGW_VOCABULARYWORDSTORE (self), iter, index);
    }

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
    }

    return is_valid;
}


static GtkTreePath*
_get_path (GtkTreeModel *tree_model,
           GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (GTK_IS_TREE_MODEL (tree_model), NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    //Declarations
    GtkTreePath *path = NULL;
    LgwVocabularyWordStore *self = NULL;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);

    if (_tree_iter_is_valid (self, iter))
    {
      path = gtk_tree_path_new_from_indices (GPOINTER_TO_INT (iter->user_data2), -1);
    }

    return path;
}


static void
_get_value (GtkTreeModel *tree_model,
            GtkTreeIter  *iter,
            gint          column,
            GValue       *value)
{
    //Sanity checks
    g_return_if_fail (GTK_IS_TREE_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (value != NULL);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    GType type = G_TYPE_INVALID;
    gint index = 0;
    LwVocabulary *vocabulary = NULL;
    LwWord *word = NULL;

 
    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = self->priv;
    type = _get_column_type (tree_model, column);
    index = GPOINTER_TO_INT (iter->user_data2);
    vocabulary = LW_VOCABULARY (self);
    word = lw_vocabulary_nth (vocabulary, index);
    g_value_init (value, type);

    if (self == NULL) goto errored;
    if (index < 0) goto errored;

    switch (column)
    {
      case LGW_VOCABULARYWORDSTORE_COLUMN_SAVED_POSITION: //G_TYPE_STRING
        g_value_take_string (value, g_strdup_printf("%d", index + 1));
        break;
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
      case LGW_VOCABULARYWORDSTORE_COLUMN_WORD:  //G_TYPE_POINTER
        g_value_set_pointer (value, word);
        break;
      default:
        g_assert_not_reached ();
    }

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
    }

    return;
}


static gboolean
_iter_next (GtkTreeModel *tree_model,
            GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint length = 0;
    gboolean has_next = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);
    vocabulary = LW_VOCABULARY (self);
    length = lw_vocabulary_length (vocabulary);
    priv = self->priv;
    index = GPOINTER_TO_INT (iter->user_data2) + 1;
    has_next = (index < length);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
    }

    return has_next;
}


static gboolean
_iter_previous (GtkTreeModel *tree_model,
                GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv;
    LwVocabulary *vocabulary = NULL;
    gint length = 0;
    gboolean has_previous = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = self->priv;
    vocabulary = LW_VOCABULARY (self);
    length = lw_vocabulary_length (vocabulary);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    has_previous = (length > 0 && index >= 0);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
    }

    return has_previous;
}


static gboolean
_iter_children (GtkTreeModel *tree_model,
                GtkTreeIter  *iter,
                GtkTreeIter  *parent)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint length = 0;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = self->priv;
    vocabulary = LW_VOCABULARY (self);
    length = lw_vocabulary_length (vocabulary);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    iter->user_data2 = GINT_TO_POINTER (index);

    if (parent == NULL && length > 0)
    {
      _initialize_tree_iter (LGW_VOCABULARYWORDSTORE (self), iter, 0);
    }

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
    }

    return (parent == NULL);
}


gboolean
_iter_has_child (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter)
{
    return FALSE;
}


static gint
_iter_n_children (GtkTreeModel *tree_model,
                                         GtkTreeIter  *iter)
{
    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint length = 0;
    gint total = 0;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);
    if (self == NULL) goto errored;
    priv = self->priv;
    vocabulary = LW_VOCABULARY (self);
    length = lw_vocabulary_length (vocabulary);

    if (iter == NULL)
    {
      total = length;
    }

errored:

    return total;
}


static gboolean
_iter_nth_child (GtkTreeModel *tree_model,
                 GtkTreeIter  *iter,
                 GtkTreeIter  *parent,
                 gint          n)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    LwVocabulary *vocabulary = NULL;
    gint length = 0;
    gint total = 0;
    gboolean exists = FALSE;

    //Initializations
    self = LGW_VOCABULARYWORDSTORE (tree_model);
    priv = self->priv;
    if (self == NULL) goto errored;
    vocabulary = LW_VOCABULARY (self);
    length = lw_vocabulary_length (vocabulary);

    if (parent == NULL)
    {
      total = length;
    }
    exists = (self != NULL && parent == NULL && total > 0 && n >= 0 && n < total);

errored:

    if (exists)
    {
      _initialize_tree_iter (LGW_VOCABULARYWORDSTORE (self), iter, n);
    }

    if (!_tree_iter_is_valid (LGW_VOCABULARYWORDSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
      exists = FALSE;
    }
    
    return exists;
}


static gboolean
_iter_parent (GtkTreeModel *tree_model,
                                     GtkTreeIter  *iter,
                                     GtkTreeIter  *child)
{
    return FALSE;
}


static void
_ref_node (GtkTreeModel *tree_model,
           GtkTreeIter  *iter)
{
    //No-op
}


static void
_unref_node (GtkTreeModel *tree_model,
             GtkTreeIter  *iter)
{
    //No-op
}


void
lgw_vocabularywordstore_implement_treemodel_interface (GtkTreeModelIface *iface)
{
    iface->get_flags = _get_flags;
    iface->get_n_columns = _get_n_columns;
    iface->get_column_type = _get_column_type;
    iface->get_iter = _get_iter;
    iface->get_path = _get_path;
    iface->get_value = _get_value;
    iface->iter_next = _iter_next;
    iface->iter_previous = _iter_previous;
    iface->iter_children = _iter_children;
    iface->iter_has_child = _iter_has_child;
    iface->iter_n_children = _iter_n_children;
    iface->iter_nth_child = _iter_nth_child;
    iface->iter_parent = _iter_parent;
}


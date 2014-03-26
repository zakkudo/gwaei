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
//! @file vocabularyliststore-treemodel-interface.c
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
lgw_vocabularyliststore_initialize_tree_iter (LgwVocabularyListStore *self,
                                              GtkTreeIter            *iter,
                                              gint                    index_)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (self), 0);

    //Declarations
    LgwVocabularyListStoreClass *klass = NULL;
 
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
_tree_iter_is_valid (LgwVocabularyListStore *self,
                     GtkTreeIter            *iter)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LgwVocabularyListStorePrivate *priv = NULL;
    gint index = 0;
    gint length = 0;

    //Initializations
    priv = self->priv;
    index = GPOINTER_TO_INT (iter->user_data2);
    length = lgw_vocabularyliststore_length (self);

    return (iter->stamp > -1 && iter->stamp > valid_stamp && index >= 0 && index < length);
}


void
lgw_vocabularyliststore_invalidate_old_timestamps ()
{
  valid_stamp = stamp - 1;
}


static GtkTreeModelFlags
_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_LIST_ONLY;
}


static gint
_get_n_columns (GtkTreeModel *tree_model)
{
    return TOTAL_LGW_VOCABULARYLISTSTORE_COLUMNS;
}


static GType
_get_column_type (GtkTreeModel *tree_model,
                  gint          index_)
{
    GType type = G_TYPE_INVALID;

    switch (index_)
    {
      case LGW_VOCABULARYLISTSTORE_COLUMN_POSITION:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_SAVED_POSITION:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_NAME:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_STYLE_WEIGHT:
        type = G_TYPE_INT;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_TOTAL_WORDS:
        type = G_TYPE_STRING;
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT:
        type = G_TYPE_OBJECT;
        break;
    }

    return type;
}


static gboolean
_get_iter (GtkTreeModel *tree_model,
           GtkTreeIter  *iter,
           GtkTreePath  *path)
{
    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gboolean is_valid = FALSE;
    gint depth = 0;
    gint index = 0;
    gint* indicies = NULL;
    gint length = 0;

    if (tree_model == NULL) goto errored;
    if (path == NULL) goto errored;
    if (iter == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = self->priv;
    length = lgw_vocabularyliststore_length (self);
    indicies = gtk_tree_path_get_indices_with_depth (path, &depth);
    if (indicies != NULL)
    {
      index = indicies[0];
    }
    is_valid = (depth == 1 && index >= 0 && index < length);

    if (iter != NULL && is_valid)
    {
      lgw_vocabularyliststore_initialize_tree_iter (LGW_VOCABULARYLISTSTORE (self), iter, index);
    }

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (self), iter))
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
    LgwVocabularyListStore *self = NULL;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);

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
    g_return_val_if_fail (GTK_IS_TREE_MODEL (tree_model), NULL);
    g_return_if_fail (iter != NULL);
    g_return_if_fail (value != NULL);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    GType type = G_TYPE_INVALID;
    gint index = 0;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;

 
    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = self->priv;
    type = _get_column_type (tree_model, column);
    index = GPOINTER_TO_INT (iter->user_data2);
    vocabulary_word_store = lgw_vocabularyliststore_nth (self, index);
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    g_value_init (value, type);

    if (vocabulary_word_store == NULL) goto errored;

    switch (column)
    {
      case LGW_VOCABULARYLISTSTORE_COLUMN_POSITION: //G_TYPE_STRING
        g_value_take_string (value, g_strdup_printf ("%d", vocabulary->row.current_index + 1));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_SAVED_POSITION: //G_TYPE_STRING
        g_value_take_string (value, g_strdup_printf ("%d", vocabulary->row.saved_index + 1));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_NAME: //G_TYPE_STRING
        g_value_set_string (value, lw_vocabulary_get_filename (vocabulary));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME: //G_TYPE_STRING
        g_value_set_string (value, lw_vocabulary_get_filename (vocabulary));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_STYLE_WEIGHT: //G_TYPE_INT
        {
          gint weight = PANGO_WEIGHT_NORMAL;
          if (lw_vocabulary_has_changes (vocabulary))
          {
            weight = PANGO_WEIGHT_BOLD;
          }
          g_value_set_int (value, weight);
        }
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_TOTAL_WORDS: //G_TYPE_STRING
        g_value_take_string (value, g_strdup_printf ("%d", lw_vocabulary_length (vocabulary)));
        break;
      case LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT: //G_TYPE_OBJECT
        g_value_set_object (value, vocabulary_word_store);
        break;
    }

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (self), iter))
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
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gboolean has_next = FALSE;
    gint index = 0;
    gint length = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = self->priv;
    index = GPOINTER_TO_INT (iter->user_data2) + 1;
    length = lgw_vocabularyliststore_length (self);
    has_next = (index < length);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (self), iter))
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
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv;
    gboolean has_previous = FALSE;
    gint index = 0;
    gint length = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = self->priv;
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    length = lgw_vocabularyliststore_length (self);
    has_previous = (length > 0 && index >= 0);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (self), iter))
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
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gint index = 0;
    gint length = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = self->priv;
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    iter->user_data2 = GINT_TO_POINTER (index);
    length = lgw_vocabularyliststore_length (self);

    if (parent == NULL && length > 0)
    {
      lgw_vocabularyliststore_initialize_tree_iter (LGW_VOCABULARYLISTSTORE (self), iter, 0);
    }

errored:

    if (!_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
    }

    return (parent == NULL);
}


static gboolean
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
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gint total = 0;
    gint length = 0;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);
    if (self == NULL) goto errored;
    priv = self->priv;
    length = lgw_vocabularyliststore_length (self);

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
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    gint total = 0;
    gboolean exists = FALSE;
    gint length = 0;

    //Initializations
    self = LGW_VOCABULARYLISTSTORE (tree_model);
    priv = self->priv;
    if (self == NULL) goto errored;
    length = lgw_vocabularyliststore_length (self);

    if (parent == NULL)
    {
      total = length;
    }
    exists = (self != NULL && parent == NULL && total > 0 && n >= 0 && n < total);

errored:

    if (exists)
    {
      lgw_vocabularyliststore_initialize_tree_iter (LGW_VOCABULARYLISTSTORE (self), iter, n);
    }

    if (!_tree_iter_is_valid (LGW_VOCABULARYLISTSTORE (self), iter))
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
lgw_vocabularyliststore_ref_node (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter)
{
    //No-op
}


static void
lgw_vocabularyliststore_unref_node (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //No-op
}


void
lgw_vocabularyliststore_implement_treemodel_interface (GtkTreeModelIface *iface)
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


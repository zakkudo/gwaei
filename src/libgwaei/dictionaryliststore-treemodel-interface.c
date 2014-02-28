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
//! @file dictionarylist-treemodel-interface.c
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

#include <libgwaei/dictionaryliststore-private.h>

static gint stamp = 0;
static gint valid_stamp = 0;


static void
_initialize_tree_iter (LgwDictionaryListStore *self,
                       GtkTreeIter       *iter,
                       gint               index_)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_DICTIONARYLISTSTORE (self), 0);

    //Declarations
    LgwDictionaryListStoreClass *klass = NULL;

    //Initializations
    iter->stamp = stamp;
    iter->user_data = self;
    iter->user_data2 = GINT_TO_POINTER (index);

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
_tree_iter_is_valid (LgwDictionaryListStore *self,
                     GtkTreeIter            *iter)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_DICTIONARYLISTSTORE (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    gint index = 0;
    gint total = 0;

    //Initializations
    index = GPOINTER_TO_INT (iter->user_data2);
    total = lw_dictionarylist_get_total (LW_DICTIONARYLIST (self));

    return (iter->stamp > -1 && iter->stamp > valid_stamp && index >= 0 && index < total);
}


void
lgw_dictionaryliststore_invalidate_old_timestamps ()
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
    return TOTAL_LGW_DICTIONARYLISTSTORE_COLUMNS;
}


static GType
_get_column_type (GtkTreeModel *tree_model,
                  gint          index_)
{
    GType type = G_TYPE_INVALID;

    switch (index_)
    {
      case LGW_DICTIONARYLISTSTORE_COLUMN_ICON_NAME:    //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_POSITION:     //G_TYPE_STRING
        type = G_TYPE_INT;
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_NAME:         //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_LONG_NAME:    //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_ENGINE:       //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_SHORTCUT:     //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_SELECTED:     //G_TYPE_BOOLEAN
        type = G_TYPE_BOOLEAN;
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_DICTIONARY:   //G_TYPE_OBJECT
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
    LwDictionaryList *self = NULL;
    gboolean is_valid = FALSE;
    gint depth = 0;
    gint index = 0;
    gint* indicies = NULL;
    gint total = 0;

    if (tree_model == NULL) goto errored;
    if (path == NULL) goto errored;
    if (iter == NULL) goto errored;

    //Initializations
    self = LW_DICTIONARYLIST (tree_model);
    indicies = gtk_tree_path_get_indices_with_depth (path, &depth);
    if (indicies != NULL)
    {
      index = indicies[0];
    }
    total = lw_dictionarylist_get_total (LW_DICTIONARYLIST (tree_model));
    is_valid = (depth == 1 && index >= 0 && index < total);

    if (iter != NULL && is_valid)
    {
      _initialize_tree_iter (LGW_DICTIONARYLISTSTORE (self), iter, index);
    }

errored:

    if (!_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (self), iter))
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
    LgwDictionaryListStore *self = NULL;

    //Initializations
    self = LGW_DICTIONARYLISTSTORE (tree_model);

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
    LwDictionaryList *self = NULL;
    GType type = G_TYPE_INVALID;
    gint index = 0;
    LwDictionary *dictionary = NULL;
 
    //Initializations
    self = LW_DICTIONARYLIST (tree_model);
    type = _get_column_type (tree_model, column);
    index = GPOINTER_TO_INT (iter->user_data2);
    dictionary = lw_dictionarylist_get_dictionary_by_position (LW_DICTIONARYLIST (tree_model), index);
    g_value_unset (value);
    g_value_init (value, type);

    if (dictionary == NULL) goto errored;

    switch (column)
    {
      case LGW_DICTIONARYLISTSTORE_COLUMN_ICON_NAME:    //G_TYPE_STRING
        {
          const gchar* ICON_NAME = NULL;
          if (index == 0)
          {
            ICON_NAME = "emblem-favorite";
          }
          g_value_set_static_string (value, ICON_NAME);
        }
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_POSITION:     //G_TYPE_STRING
        {
          g_value_set_int (value, index);
        }
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_NAME:         //G_TYPE_STRING
        {
          const gchar *NAME = lw_dictionary_get_name(dictionary);
          g_value_set_string (value, NAME);
        }
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_LONG_NAME:    //G_TYPE_STRING
        {
          const gchar *NAME = lw_dictionary_get_name (dictionary);
          gchar *long_name = g_strdup_printf(gettext("%s Dictionary"), NAME);
          g_value_take_string (value, long_name);
        }
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_ENGINE:       //G_TYPE_STRING
        {
          gchar *id = lw_dictionary_build_id (dictionary);
          g_value_take_string (value, id);
        }
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_SHORTCUT:     //G_TYPE_STRING
        {
          gchar *shortcut = g_strdup_printf ("Ctrl-%d", index);
          g_value_take_string (value, shortcut);
        }
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_SELECTED:     //G_TYPE_BOOLEAN
        {
          gboolean selected = lw_dictionary_is_selected (dictionary);
          g_value_set_boolean (value, selected);
        }
        break;
      case LGW_DICTIONARYLISTSTORE_COLUMN_DICTIONARY:   //G_TYPE_OBJECT
        {
          g_value_set_object (value, dictionary);
        }
        break;
    }

errored:

    if (!_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (self), iter))
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
    LwDictionaryList *self = NULL;
    gint total = 0;
    gboolean has_next = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (self);
    index = GPOINTER_TO_INT (iter->user_data2) + 1;
    has_next = (index < total);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (self), iter))
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
    LwDictionaryList *self = NULL;
    gint total = 0;
    gboolean has_previous = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (self);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    has_previous = (total > 0 && index >= 0);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (self), iter))
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
    LwDictionaryList *self = NULL;
    gint total = 0;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    self = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (self);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    iter->user_data2 = GINT_TO_POINTER (index);

    if (parent == NULL && total > 0)
    {
      _initialize_tree_iter (LGW_DICTIONARYLISTSTORE (self), iter, 0);
    }

errored:

    if (!_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (self), iter))
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
    LwDictionaryList *self = NULL;
    gint total = 0;

    //Initializations
    self = LW_DICTIONARYLIST (tree_model);
    if (self == NULL) goto errored;

    if (iter == NULL)
    {
      total = lw_dictionarylist_get_total (self);
    }

errored:

    if (!_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (self), iter))
    {
      _invalidate_tree_iter (iter);
    }
    
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
    LwDictionaryList *self = NULL;
    gint total = 0;
    gboolean exists = FALSE;

    //Initializations
    self = LW_DICTIONARYLIST (tree_model);
    if (self == NULL) goto errored;

    if (parent == NULL)
    {
      total = lw_dictionarylist_get_total (self);
    }
    exists = (self != NULL && parent == NULL && total > 0 && n >= 0 && n < total);

errored:

    if (exists)
    {
      _initialize_tree_iter (LGW_DICTIONARYLISTSTORE (self), iter, n);
    }

    if (!_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (self), iter))
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
lgw_dictionaryliststore_ref_node (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter)
{
    //No-op
}


static void
lgw_dictionaryliststore_unref_node (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //No-op
}


void
lgw_dictionaryliststore_implement_treemodel_interface (GtkTreeModelIface *iface)
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


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
//! @file dictionarylist-iter.c
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

void
lgw_dictionaryliststore_init_interface (GtkTreeModelIface *iface)
{
    iface->get_flags = lgw_dictionaryliststore_get_flags;
    iface->get_n_columns = lgw_dictionaryliststore_get_n_columns;
    iface->get_column_type = lgw_dictionaryliststore_get_column_type;
    iface->get_iter = lgw_dictionaryliststore_get_iter;
    iface->get_path = lgw_dictionaryliststore_get_path;
    iface->get_value = lgw_dictionaryliststore_get_value;
    iface->iter_next = lgw_dictionaryliststore_iter_next;
    iface->iter_previous = lgw_dictionaryliststore_iter_previous;
    iface->iter_children = lgw_dictionaryliststore_iter_children;
    iface->iter_has_child = lgw_dictionaryliststore_iter_has_child;
    iface->iter_n_children = lgw_dictionaryliststore_iter_n_children;
    iface->iter_nth_child = lgw_dictionaryliststore_iter_nth_child;
    iface->iter_parent = lgw_dictionaryliststore_iter_parent;
}



void
lgw_dictionaryliststore_initialize_tree_iter (LgwDictionaryListStore *dictionary_list_store,
                                         GtkTreeIter       *iter,
                                         gint               index_)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_DICTIONARYLISTSTORE (dictionary_list_store), 0);

    //Declarations
    LgwDictionaryListStoreClass *klass = NULL;

    //Initializations
    iter->stamp = stamp;
    iter->user_data = dictionary_list_store;
    iter->user_data2 = GINT_TO_POINTER (index);

    stamp++;
}


void
lgw_dictionaryliststore_invalidate_tree_iter (GtkTreeIter *iter)
{
    //Sanity checks
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(GtkTreeIter));
    iter->stamp = -1;
}


gboolean
lgw_dictionaryliststore_tree_iter_is_valid (LgwDictionaryListStore *dictionary_list_store,
                                       GtkTreeIter       *iter)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_DICTIONARYLISTSTORE (dictionary_list_store), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    gint index = 0;
    gint total = 0;

    //Initializations
    index = GPOINTER_TO_INT (iter->user_data2);
    total = lw_dictionarylist_get_total (LW_DICTIONARYLIST (dictionary_list_store));

    return (iter->stamp > -1 && iter->stamp > valid_stamp && index >= 0 && index < total);
}


void
lgw_dictionaryliststore_invalidate_old_timestamps ()
{
  valid_stamp = stamp - 1;
}


GtkTreeModelFlags
lgw_dictionaryliststore_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_LIST_ONLY;
}


gint
lgw_dictionaryliststore_get_n_columns (GtkTreeModel *tree_model)
{
    return TOTAL_LGW_DICTIONARYLISTSTORE_COLUMNS;
}


GType
lgw_dictionaryliststore_get_column_type (GtkTreeModel *tree_model,
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


gboolean
lgw_dictionaryliststore_get_iter (GtkTreeModel *tree_model,
                             GtkTreeIter  *iter,
                             GtkTreePath  *path)
{
    //Declarations
    LwDictionaryList *dictionary_list_store = NULL;
    gboolean is_valid = FALSE;
    gint depth = 0;
    gint index = 0;
    gint* indicies = NULL;
    gint total = 0;

    if (tree_model == NULL) goto errored;
    if (path == NULL) goto errored;
    if (iter == NULL) goto errored;

    //Initializations
    dictionary_list_store = LW_DICTIONARYLIST (tree_model);
    indicies = gtk_tree_path_get_indices_with_depth (path, &depth);
    if (indicies != NULL)
    {
      index = indicies[0];
    }
    total = lw_dictionarylist_get_total (LW_DICTIONARYLIST (tree_model));
    is_valid = (depth == 1 && index >= 0 && index < total);

    if (iter != NULL && is_valid)
    {
      lgw_dictionaryliststore_initialize_tree_iter (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter, index);
    }

errored:

    if (!lgw_dictionaryliststore_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter))
    {
      lgw_dictionaryliststore_invalidate_tree_iter (iter);
    }

    return is_valid;
}


GtkTreePath*
lgw_dictionaryliststore_get_path (GtkTreeModel *tree_model,
                             GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (GTK_IS_TREE_MODEL (tree_model), NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    //Declarations
    GtkTreePath *path = NULL;
    LgwDictionaryListStore *dictionary_list_store = NULL;

    //Initializations
    dictionary_list_store = LGW_DICTIONARYLISTSTORE (tree_model);

    if (lgw_dictionaryliststore_tree_iter_is_valid (dictionary_list_store, iter))
    {
      path = gtk_tree_path_new_from_indices (GPOINTER_TO_INT (iter->user_data2), -1);
    }

    return path;
}


void
lgw_dictionaryliststore_get_value (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter,
                              gint          column,
                              GValue       *value)
{
    //Sanity checks
    g_return_if_fail (GTK_IS_TREE_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (value != NULL);

    //Declarations
    LwDictionaryList *dictionary_list_store = NULL;
    GType type = G_TYPE_INVALID;
    gint index = 0;
    LwDictionary *dictionary = NULL;
 
    //Initializations
    dictionary_list_store = LW_DICTIONARYLIST (tree_model);
    type = lgw_dictionaryliststore_get_column_type (tree_model, column);
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

    if (!lgw_dictionaryliststore_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter))
    {
      lgw_dictionaryliststore_invalidate_tree_iter (iter);
    }

    return;
}


gboolean
lgw_dictionaryliststore_iter_next (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list_store = NULL;
    gint total = 0;
    gboolean has_next = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    dictionary_list_store = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (dictionary_list_store);
    index = GPOINTER_TO_INT (iter->user_data2) + 1;
    has_next = (index < total);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_dictionaryliststore_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter))
    {
      lgw_dictionaryliststore_invalidate_tree_iter (iter);
    }

    return has_next;
}


gboolean
lgw_dictionaryliststore_iter_previous (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list_store = NULL;
    gint total = 0;
    gboolean has_previous = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    dictionary_list_store = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (dictionary_list_store);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    has_previous = (total > 0 && index >= 0);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_dictionaryliststore_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter))
    {
      lgw_dictionaryliststore_invalidate_tree_iter (iter);
    }

    return has_previous;
}


gboolean
lgw_dictionaryliststore_iter_children (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter,
                                  GtkTreeIter  *parent)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list_store = NULL;
    gint total = 0;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    dictionary_list_store = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (dictionary_list_store);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    iter->user_data2 = GINT_TO_POINTER (index);

    if (parent == NULL && total > 0)
    {
      lgw_dictionaryliststore_initialize_tree_iter (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter, 0);
    }

errored:

    if (!lgw_dictionaryliststore_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter))
    {
      lgw_dictionaryliststore_invalidate_tree_iter (iter);
    }

    return (parent == NULL);
}


gboolean
lgw_dictionaryliststore_iter_has_child (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter)
{
    return FALSE;
}


gint
lgw_dictionaryliststore_iter_n_children (GtkTreeModel *tree_model,
                                    GtkTreeIter  *iter)
{
    //Declarations
    LwDictionaryList *dictionary_list_store = NULL;
    gint total = 0;

    //Initializations
    dictionary_list_store = LW_DICTIONARYLIST (tree_model);
    if (dictionary_list_store == NULL) goto errored;

    if (iter == NULL)
    {
      total = lw_dictionarylist_get_total (dictionary_list_store);
    }

errored:

    if (!lgw_dictionaryliststore_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter))
    {
      lgw_dictionaryliststore_invalidate_tree_iter (iter);
    }
    
    return total;
}


gboolean
lgw_dictionaryliststore_iter_nth_child (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter,
                                   GtkTreeIter  *parent,
                                   gint          n)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list_store = NULL;
    gint total = 0;
    gboolean exists = FALSE;

    //Initializations
    dictionary_list_store = LW_DICTIONARYLIST (tree_model);
    if (dictionary_list_store == NULL) goto errored;

    if (parent == NULL)
    {
      total = lw_dictionarylist_get_total (dictionary_list_store);
    }
    exists = (dictionary_list_store != NULL && parent == NULL && total > 0 && n >= 0 && n < total);

errored:

    if (exists)
    {
      lgw_dictionaryliststore_initialize_tree_iter (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter, n);
    }

    if (!lgw_dictionaryliststore_tree_iter_is_valid (LGW_DICTIONARYLISTSTORE (dictionary_list_store), iter))
    {
      lgw_dictionaryliststore_invalidate_tree_iter (iter);
      exists = FALSE;
    }
    
    return exists;
}


gboolean
lgw_dictionaryliststore_iter_parent (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter,
                                GtkTreeIter  *child)
{
    return FALSE;
}


void
lgw_dictionaryliststore_ref_node (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter)
{
    //No-op
}


void
lgw_dictionaryliststore_unref_node (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //No-op
}


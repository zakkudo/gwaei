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

#include <libgwaei/dictionarylist-private.h>


void
lgw_dictionarylist_changed_cb (LgwDictionaryList *dictionary_list,
                               gint               position,
                               gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LgwDictionaryListPrivate *priv = NULL;
    GtkTreeIter iter;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;
    lgw_dictionarylist_initialize_tree_iter (dictionary_list, &iter, position);
    if (!lgw_dictionarylist_tree_iter_is_valid (dictionary_list, &iter)) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list),
      "row-changed",
      path,
      &iter,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}


void
lgw_dictionarylist_inserted_cb (LgwDictionaryList *dictionary_list,
                                gint               position,
                                gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LgwDictionaryListPrivate *priv = NULL;
    GtkTreeIter iter;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;
    lgw_dictionarylist_initialize_tree_iter (dictionary_list, &iter, position);
    if (!lgw_dictionarylist_tree_iter_is_valid (dictionary_list, &iter)) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list),
      "row-inserted",
      path,
      &iter,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}

void
lgw_dictionarylist_deleted_cb (LgwDictionaryList *dictionary_list,
                               gint               position,
                               gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LgwDictionaryListPrivate *priv = NULL;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list->priv;
    path = gtk_tree_path_new_from_indices (position, -1);
    if (path == NULL) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list),
      "row-deleted",
      path,
      NULL
    );

errored:

    if (path != NULL) gtk_tree_path_free (path); path = NULL;
}

void
lgw_dictionarylist_reordered_cb (LgwDictionaryList *dictionary_list,
                                 gint              *new_order,
                                 gpointer           data)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LgwDictionaryListPrivate *priv = NULL;
    GtkTreePath *path = NULL;

    //Initializations
    priv = dictionary_list->priv;
    path = gtk_tree_path_new_from_indices (-1);
    if (path == NULL) goto errored;

    g_signal_emit_by_name (G_OBJECT (dictionary_list),
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
lgw_dictionarylist_connect_signals (LgwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LgwDictionaryListPrivate *priv = NULL;

    //Initializations
    priv = dictionary_list->priv;

    if (priv->data.signalid[SIGNALID_CHANGED] == 0)
    {
      priv->data.signalid[SIGNALID_CHANGED] = g_signal_connect (
        G_OBJECT (dictionary_list),
        "changed",
        G_CALLBACK (lgw_dictionarylist_changed_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_INSERTED] == 0)
    {
      priv->data.signalid[SIGNALID_INSERTED] = g_signal_connect (
        G_OBJECT (dictionary_list),
        "inserted",
        G_CALLBACK (lgw_dictionarylist_inserted_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_DELETED] == 0)
    {
      priv->data.signalid[SIGNALID_DELETED] = g_signal_connect (
        G_OBJECT (dictionary_list),
        "deleted",
        G_CALLBACK (lgw_dictionarylist_deleted_cb),
        NULL
      );
    }

    if (priv->data.signalid[SIGNALID_REORDERED] == 0)
    {
      priv->data.signalid[SIGNALID_REORDERED] = g_signal_connect (
        G_OBJECT (dictionary_list),
        "reordered",
        G_CALLBACK (lgw_dictionarylist_reordered_cb),
        NULL
      );
    }
}


void
lgw_dictionarylist_disconnect_signals (LgwDictionaryList *dictionary_list)
{
    //Sanity checks
    g_return_if_fail (dictionary_list != NULL);

    //Declarations
    LgwDictionaryListPrivate *priv = NULL;
    gint i = 0;

    //Initializations
    priv = dictionary_list->priv;

    for (i = 0; i < TOTAL_SIGNALIDS; i++)
    {
      if (priv->data.signalid[i] != 0)
      {
        g_signal_handler_disconnect (G_OBJECT (dictionary_list), priv->data.signalid[i]);
        priv->data.signalid[i] = 0;
      }
    }
}













GtkTreeModelFlags
lgw_dictionarylist_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_LIST_ONLY;
}


gint
lgw_dictionarylist_get_n_columns (GtkTreeModel *tree_model)
{
    return TOTAL_LGW_DICTIONARYLIST_COLUMNS;
}


GType
lgw_dictionarylist_get_column_type (GtkTreeModel *tree_model,
                                   gint          index_)
{
    GType type = G_TYPE_INVALID;

    switch (index_)
    {
      case LGW_DICTIONARYLIST_COLUMN_ICON_NAME:    //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLIST_COLUMN_POSITION:     //G_TYPE_STRING
        type = G_TYPE_INT;
        break;
      case LGW_DICTIONARYLIST_COLUMN_NAME:         //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLIST_COLUMN_LONG_NAME:    //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLIST_COLUMN_ENGINE:       //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLIST_COLUMN_SHORTCUT:     //G_TYPE_STRING
        type = G_TYPE_STRING;
        break;
      case LGW_DICTIONARYLIST_COLUMN_SELECTED:     //G_TYPE_BOOLEAN
        type = G_TYPE_BOOLEAN;
        break;
      case LGW_DICTIONARYLIST_COLUMN_DICTIONARY:   //G_TYPE_OBJECT
        type = G_TYPE_OBJECT;
        break;
    }

    return type;
}


gboolean
lgw_dictionarylist_get_iter (GtkTreeModel *tree_model,
                             GtkTreeIter  *iter,
                             GtkTreePath  *path)
{
    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    gboolean is_valid = FALSE;
    gint depth = 0;
    gint index = 0;
    gint* indicies = NULL;
    gint total = 0;

    if (tree_model == NULL) goto errored;
    if (path == NULL) goto errored;
    if (iter == NULL) goto errored;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (tree_model);
    indicies = gtk_tree_path_get_indices_with_depth (path, &depth);
    if (indicies != NULL)
    {
      index = indicies[0];
    }
    total = lw_dictionarylist_get_total (LW_DICTIONARYLIST (tree_model));
    is_valid = (depth == 1 && index >= 0 && index < total);

    if (iter != NULL && is_valid)
    {
      lgw_dictionarylist_initialize_tree_iter (LGW_DICTIONARYLIST (dictionary_list), iter, index);
    }

errored:

    if (!lgw_dictionarylist_tree_iter_is_valid (LGW_DICTIONARYLIST (dictionary_list), iter))
    {
      lgw_dictionarylist_invalidate_tree_iter (iter);
    }

    return is_valid;
}


GtkTreePath*
lgw_dictionarylist_get_path (GtkTreeModel *tree_model,
                             GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (tree_model != NULL, NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    //Declarations
    GtkTreePath *path = NULL;
    LgwDictionaryList *dictionary_list = NULL;

    //Initializations
    dictionary_list = LGW_DICTIONARYLIST (tree_model);

    if (lgw_dictionarylist_tree_iter_is_valid (dictionary_list, iter))
    {
      path = gtk_tree_path_new_from_indices (GPOINTER_TO_INT (iter->user_data2), -1);
    }

    return path;
}


void
lgw_dictionarylist_get_value (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter,
                              gint          column,
                              GValue       *value)
{
    //Sanity checks
    g_return_if_fail (tree_model != NULL);
    g_return_if_fail (iter != NULL);
    g_return_if_fail (value != NULL);

    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    GType type = G_TYPE_INVALID;
    gint index = 0;
    LwDictionary *dictionary = NULL;
 
    //Initializations
    dictionary_list = LW_DICTIONARYLIST (tree_model);
    type = lgw_dictionarylist_get_column_type (tree_model, column);
    index = GPOINTER_TO_INT (iter->user_data2);
    dictionary = lw_dictionarylist_get_dictionary_by_position (LW_DICTIONARYLIST (tree_model), index);
    g_value_unset (value);
    g_value_init (value, type);

    if (dictionary == NULL) goto errored;

    switch (column)
    {
      case LGW_DICTIONARYLIST_COLUMN_ICON_NAME:    //G_TYPE_STRING
        {
          const gchar* ICON_NAME = NULL;
          if (index == 0)
          {
            ICON_NAME = "emblem-favorite";
          }
          g_value_set_static_string (value, ICON_NAME);
        }
        break;
      case LGW_DICTIONARYLIST_COLUMN_POSITION:     //G_TYPE_STRING
        {
          g_value_set_int (value, index);
        }
        break;
      case LGW_DICTIONARYLIST_COLUMN_NAME:         //G_TYPE_STRING
        {
          const gchar *NAME = lw_dictionary_get_name(dictionary);
          g_value_set_string (value, NAME);
        }
        break;
      case LGW_DICTIONARYLIST_COLUMN_LONG_NAME:    //G_TYPE_STRING
        {
          const gchar *NAME = lw_dictionary_get_name (dictionary);
          gchar *long_name = g_strdup_printf(gettext("%s Dictionary"), NAME);
          g_value_take_string (value, long_name);
        }
        break;
      case LGW_DICTIONARYLIST_COLUMN_ENGINE:       //G_TYPE_STRING
        {
          gchar *id = lw_dictionary_build_id (dictionary);
          g_value_take_string (value, id);
        }
        break;
      case LGW_DICTIONARYLIST_COLUMN_SHORTCUT:     //G_TYPE_STRING
        {
          gchar *shortcut = g_strdup_printf ("Ctrl-%d", index);
          g_value_take_string (value, shortcut);
        }
        break;
      case LGW_DICTIONARYLIST_COLUMN_SELECTED:     //G_TYPE_BOOLEAN
        {
          gboolean selected = lw_dictionary_is_selected (dictionary);
          g_value_set_boolean (value, selected);
        }
        break;
      case LGW_DICTIONARYLIST_COLUMN_DICTIONARY:   //G_TYPE_OBJECT
        {
          g_value_set_object (value, dictionary);
        }
        break;
    }

errored:

    if (!lgw_dictionarylist_tree_iter_is_valid (LGW_DICTIONARYLIST (dictionary_list), iter))
    {
      lgw_dictionarylist_invalidate_tree_iter (iter);
    }

    return;
}


gboolean
lgw_dictionarylist_iter_next (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    gint total = 0;
    gboolean has_next = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (dictionary_list);
    index = GPOINTER_TO_INT (iter->user_data2) + 1;
    has_next = (index < total);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_dictionarylist_tree_iter_is_valid (LGW_DICTIONARYLIST (dictionary_list), iter))
    {
      lgw_dictionarylist_invalidate_tree_iter (iter);
    }

    return has_next;
}


gboolean
lgw_dictionarylist_iter_previous (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    gint total = 0;
    gboolean has_previous = FALSE;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (dictionary_list);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    has_previous = (total > 0 && index >= 0);
    iter->user_data2 = GINT_TO_POINTER (index);

errored:

    if (!lgw_dictionarylist_tree_iter_is_valid (LGW_DICTIONARYLIST (dictionary_list), iter))
    {
      lgw_dictionarylist_invalidate_tree_iter (iter);
    }

    return has_previous;
}


gboolean
lgw_dictionarylist_iter_children (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter,
                                  GtkTreeIter  *parent)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    gint total = 0;
    gint index = 0;

    if (tree_model == NULL) goto errored;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (tree_model);
    total = lw_dictionarylist_get_total (dictionary_list);
    index = GPOINTER_TO_INT (iter->user_data2) - 1;
    iter->user_data2 = GINT_TO_POINTER (index);

    if (parent == NULL && total > 0)
    {
      lgw_dictionarylist_initialize_tree_iter (LGW_DICTIONARYLIST (dictionary_list), iter, 0);
    }

errored:

    if (!lgw_dictionarylist_tree_iter_is_valid (LGW_DICTIONARYLIST (dictionary_list), iter))
    {
      lgw_dictionarylist_invalidate_tree_iter (iter);
    }

    return (parent == NULL);
}


gboolean
lgw_dictionarylist_iter_has_child (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter)
{
    return FALSE;
}


gint
lgw_dictionarylist_iter_n_children (GtkTreeModel *tree_model,
                                    GtkTreeIter  *iter)
{
    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    gint total = 0;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (tree_model);
    if (dictionary_list == NULL) goto errored;

    if (iter == NULL)
    {
      total = lw_dictionarylist_get_total (dictionary_list);
    }

errored:

    if (!lgw_dictionarylist_tree_iter_is_valid (LGW_DICTIONARYLIST (dictionary_list), iter))
    {
      lgw_dictionarylist_invalidate_tree_iter (iter);
    }
    
    return total;
}


gboolean
lgw_dictionarylist_iter_nth_child (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter,
                                   GtkTreeIter  *parent,
                                   gint          n)
{
    //Sanity checks
    g_return_val_if_fail (iter != NULL, FALSE);

    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    gint total = 0;
    gboolean exists = FALSE;

    //Initializations
    dictionary_list = LW_DICTIONARYLIST (tree_model);
    if (dictionary_list == NULL) goto errored;

    if (parent == NULL)
    {
      total = lw_dictionarylist_get_total (dictionary_list);
    }
    exists = (dictionary_list != NULL && parent == NULL && total > 0 && n >= 0 && n < total);

errored:

    if (exists)
    {
      lgw_dictionarylist_initialize_tree_iter (LGW_DICTIONARYLIST (dictionary_list), iter, n);
    }

    if (!lgw_dictionarylist_tree_iter_is_valid (LGW_DICTIONARYLIST (dictionary_list), iter))
    {
      lgw_dictionarylist_invalidate_tree_iter (iter);
      exists = FALSE;
    }
    
    return exists;
}


gboolean
lgw_dictionarylist_iter_parent (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter,
                                GtkTreeIter  *child)
{
    return FALSE;
}


void
lgw_dictionarylist_ref_node (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter)
{
    //No-op
}


void
lgw_dictionarylist_unref_node (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter)
{
    //No-op
}


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
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/dictionarylist-private.h>

gchar* lw_dictionary_get_name
gchar* lw_dictionary_get_dictionaryname
gchar* lw_dictionary_build_id
gboolean lw_dictionary_is_selected
gchar*  lw_dictionary_get_length
guint lw_dictiononay_get_position


GtkTreeModelFlags lw_dictionarylist_get_flags (GtkTreeModel *tree_model)
{
    return GTK_TREE_MODEL_LIST_ONLY;
}

gint  lw_dictionarylist_get_n_columns (GtkTreeModel *tree_model)
{
    
}

GType lw_dictionarylist_get_column_type (GtkTreeModel *tree_model,
                                         gint          index_)
{
}

gboolean lw_dictionarylist_get_iter (GtkTreeModel *tree_model,
                                     GtkTreeIter  *iter,
                                     GtkTreePath  *path)
{
    text = gtk_tree_path_to_string (path);
}

GtkTreePath * lw_dictionarylist_get_path (GtkTreeModel *tree_model,
                                          GtkTreeIter  *iter)
{
    return gtk_tree_path_new_from_string (path);
}

void lw_dictionarylist_get_value (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter,
                                  gint          column,
                                  GValue       *value)
{
}

gboolean lw_dictionarylist_iter_next (GtkTreeModel *tree_model,
                                      GtkTreeIter  *iter)
{
}

gboolean lw_dictioanrylist_iter_previous (GtkTreeModel *tree_model,
                                          GtkTreeIter  *iter)
{
}

gboolean lw_dictionarylist_iter_children (GtkTreeModel *tree_model,
                                          GtkTreeIter  *iter,
                                          GtkTreeIter  *parent)
{
    iter = new invaliditer();
    return FALSE;
}

gboolean lw_dictionarylist_iter_has_child (GtkTreeModel *tree_model,
                                           GtkTreeIter  *iter)
{
    gboolean hasChild = FALSE;

    if (iter == NULL)
    {
      hasChild = TRUE;
    }

    return hasChild;
}

gint lw_dictionarylist_iter_n_children (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter)
{
    ginit total = 0;

    if (iter == NULL)
    {
      total = lw_dictionarylist_get_total (tree_model);
    }
    
    return total;
}

gboolean lw_dictionarylist_iter_nth_child (GtkTreeModel *tree_model,
                                           GtkTreeIter  *iter,
                                           GtkTreeIter  *parent,
                                           gint          n)
{
    if (parent == NULL)
    {
      lw_dictionarylist_get_dictionary_by_position (tree_model, n);
    }
    else
    {
      set invalid
    }
}

gboolean lw_dictionarylist_iter_parent (GtkTreeModel *tree_model,
                                        GtkTreeIter  *iter,
                                        GtkTreeIter  *child)
{
  iter = new invaliditer();
}

void lw_dictionarylist_ref_node (GtkTreeModel *tree_model,
                                 GtkTreeIter  *iter)
{
    //No-op
}

void lw_dictionarylist_unref_node (GtkTreeModel *tree_model,
                                   GtkTreeIter  *iter)
{
    //No-op
}


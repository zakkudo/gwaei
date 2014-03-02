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
//! @file vocabularyliststore-treesortable-interface.c
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


static void
_sort_column_changed (GtkTreeSortable *sortable)
{
    g_signal_emit_by_name (G_OBJECT (sortable), "sort-coumn-changed", NULL);
}


static gboolean 
_get_sort_column_id (GtkTreeSortable *sortable,
                     gint            *sort_column_id,
                     GtkSortType     *order)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYLISTSTORE (sortable);
    priv = self->priv;

    *sort_column_id = priv->config.sort_column_id;
    *order = priv->config.order;

    return !(GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID || GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID);
}


static void     
_set_sort_column_id (GtkTreeSortable *sortable,
                     gint             sort_column_id,
                     GtkSortType      order)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYLISTSTORE (sortable);
    priv = self->priv;

    priv->config.sort_column_id = sort_column_id;
    priv->config.order = order;
}


static void 
_set_sort_func (GtkTreeSortable        *sortable,
                gint                    sort_column_id,
                GtkTreeIterCompareFunc  sort_func,
                gpointer                user_data,
                GDestroyNotify          destroy)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYLISTSTORE (sortable);
    priv = self->priv;

    priv->config.sort_func = sort_func;
}


static gint
_default_sort_func (GtkTreeModel *model,
                    GtkTreeIter  *a,
                    GtkTreeIter *b,
                    gpointer user_data)
{
    g_assert_not_reached ();
    return 0;
}


static void
_set_default_sort_func (GtkTreeSortable        *sortable,
                        GtkTreeIterCompareFunc  sort_func,
                        gpointer                user_data,
                        GDestroyNotify          destroy)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYLISTSTORE (sortable);
    priv = self->priv;

    priv->config.sort_func = _default_sort_func;
}


static gboolean
_has_default_sort_func (GtkTreeSortable *sortable)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYLISTSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyListStore *self = NULL;
    LgwVocabularyListStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYLISTSTORE (sortable);
    priv = self->priv;

    return TRUE;
}


void
lgw_vocabularyliststore_implement_treesortable_interface (GtkTreeSortableIface *iface)
{
    iface->sort_column_changed = _sort_column_changed;
    iface->get_sort_column_id = _get_sort_column_id;
    iface->set_sort_column_id = _set_sort_column_id;
    iface->set_sort_func = _set_sort_func;
    iface->set_default_sort_func = _set_default_sort_func;
    iface->has_default_sort_func = _has_default_sort_func;
}



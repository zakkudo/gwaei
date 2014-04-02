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
//! @file vocabularywordstore-treesortable-interface.c
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

#include <libgwaei/vocabularywordstore-private.h>


static void
_sort_column_changed (GtkTreeSortable *sortable)
{
printf("BREAK _sort_column_changed\n");
}


static gboolean 
_get_sort_column_id (GtkTreeSortable *sortable,
                     gint            *sort_column_id,
                     GtkSortType     *order)
{
printf("BREAK _get_sort_column_id\n");
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYWORDSTORE (sortable);
    priv = self->priv;

    if (sort_column_id != NULL)
    {
      *sort_column_id = priv->config.sort_column_id;
    }

    if (order != NULL)
    {
      *order = priv->config.order;
    }

    return (*sort_column_id != GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID && *sort_column_id != GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID);
}


static void     
_set_sort_column_id (GtkTreeSortable *sortable,
                     gint             sort_column_id,
                     GtkSortType      order)
{
    printf("BREAK _set_sort_column_id %d %d\n", sort_column_id, order);

    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYWORDSTORE (sortable);
    priv = self->priv;

    priv->config.sort_column_id = sort_column_id;
    priv->config.order = order;

    g_signal_emit_by_name (sortable, "sort-column-changed", NULL);

    lgw_vocabularywordstore_sort (self);
}


static void 
_set_sort_func (GtkTreeSortable        *sortable,
                gint                    sort_column_id,
                GtkTreeIterCompareFunc  sort_func,
                gpointer                user_data,
                GDestroyNotify          destroy)
{
printf("BREAK _set_sort_func\n");
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYWORDSTORE (sortable);
    priv = self->priv;

    priv->config.sort_func[sort_column_id] = sort_func;
}


static void
_set_default_sort_func (GtkTreeSortable        *sortable,
                        GtkTreeIterCompareFunc  sort_func,
                        gpointer                user_data,
                        GDestroyNotify          destroy)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYWORDSTORE (sortable);
    priv = self->priv;

    priv->config.default_sort_func = sort_func;
}


static gboolean
_has_default_sort_func (GtkTreeSortable *sortable)
{
printf("BREAK _has_default_sort_func\n");
    //Sanity checks
    g_return_val_if_fail (LGW_IS_VOCABULARYWORDSTORE (sortable), FALSE);

    //Declarations
    LgwVocabularyWordStore *self = NULL;
    LgwVocabularyWordStorePrivate *priv = NULL;
    
    //Initializations
    self = LGW_VOCABULARYWORDSTORE (sortable);
    priv = self->priv;

    return (priv->config.default_sort_func != NULL);
}


gint
lgw_vocabularywordstore_kanji_compare_func (GtkTreeModel *model,
                                            GtkTreeIter  *a,
                                            GtkTreeIter  *b,
                                            gpointer      user_data)
{
printf("BREAK lgw_vocabularywordstore_kanji_compare_func %d\n", lw_vocabulary_length (LW_VOCABULARY (model)));
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (model));

    //Declarations
    LwWord *wa = NULL;
    LwWord *wb = NULL;
    const gchar *ka = NULL;
    const gchar *kb = NULL;
    gint result = 0;

    //Initializations
    gtk_tree_model_get (model, a, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wa, -1);
    if (wa == NULL) goto errored;
    gtk_tree_model_get (model, b, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wb, -1);
    if (wb == NULL) goto errored;
    ka = lw_word_get_kanji (wa);
    kb = lw_word_get_kanji (wb);

    printf("BREAK lgw_vocabularywordstore_kanji_compare_func\n");

errored:

    result = g_strcmp0 (ka, kb);

    return result;
}


gint
lgw_vocabularywordstore_reading_compare_func (GtkTreeModel *model,
                                              GtkTreeIter  *a,
                                              GtkTreeIter  *b,
                                              gpointer      user_data)
{
printf("BREAK lgw_vocabularywordstore_reading_compare_func\n");
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (model));

    //Declarations
    LwWord *wa = NULL;
    LwWord *wb = NULL;
    const gchar *ra = NULL;
    const gchar *rb = NULL;
    gint result = 0;

    //Initializations
    gtk_tree_model_get (model, a, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wa, -1);
    if (wa == NULL) goto errored;
    gtk_tree_model_get (model, b, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wb, -1);
    if (wb == NULL) goto errored;
    ra = lw_word_get_reading (wa);
    rb = lw_word_get_reading (wb);

    printf("BREAK1 lgw_vocabularywordstore_reading_compare_func\n");

errored:

    result = g_strcmp0 (ra, rb);

    return result;
}


gint
lgw_vocabularywordstore_definition_compare_func (GtkTreeModel *model,
                                                 GtkTreeIter  *a,
                                                 GtkTreeIter  *b,
                                                 gpointer      user_data)
{
printf("BREAK lgw_vocabularywordstore_definition_compare_func\n");
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (model));

    //Declarations
    LwWord *wa = NULL;
    LwWord *wb = NULL;
    const gchar *da = NULL;
    const gchar *db = NULL;
    gint result = 0;

    //Initializations
    gtk_tree_model_get (model, a, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wa, -1);
    if (wa == NULL) goto errored;
    gtk_tree_model_get (model, b, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wb, -1);
    if (wb == NULL) goto errored;
    da = lw_word_get_definition (wa);
    db = lw_word_get_definition (wb);

    printf("BREAK1 lgw_vocabularywordstore_definition_compare_func\n");

errored:

    result = g_strcmp0 (da, db);

    return result;
}


gint
lgw_vocabularywordstore_saved_position_compare_func (GtkTreeModel *model,
                                                     GtkTreeIter  *a,
                                                     GtkTreeIter  *b,
                                                     gpointer      user_data)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_VOCABULARYWORDSTORE (model));

    printf("BREAK lgw_vocabularywordstore_saved_position_compare_func\n");

    //Declarations
    LwWord *wa = NULL;
    LwWord *wb = NULL;
    gint pa = -1;
    gint pb = -1;
    gint result = 0;

    //Initializations
    gtk_tree_model_get (model, a, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wa, -1);
    if (wa == NULL) goto errored;
    gtk_tree_model_get (model, b, LGW_VOCABULARYWORDSTORE_COLUMN_WORD, &wb, -1);
    if (wb == NULL) goto errored;
    pa = wa->row.saved_index;
    pb = wb->row.saved_index;

errored:

    if (pa < 0 && pb >= 0) result = 1;
    else if (pb < 00 && pa >= 0) result = -1;
    else result = pa - pb;

    return result;
}


void
lgw_vocabularywordstore_implement_treesortable_interface (GtkTreeSortableIface *iface)
{
    iface->sort_column_changed = _sort_column_changed;
    iface->get_sort_column_id = _get_sort_column_id;
    iface->set_sort_column_id = _set_sort_column_id;
    iface->set_sort_func = _set_sort_func;
    iface->set_default_sort_func = _set_default_sort_func;
    iface->has_default_sort_func = _has_default_sort_func;
}


//!
//! @file editable_table.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include "gettext.h"

#include "editable-table.h"

#define lw_editable_table_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_EDITABLE_TABLE, LwEditableTableClass)
#define lw_editable_table_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_editable_table_get_class(self), LW_TYPE_EDITABLE_TABLE, LwEditableTableClassPrivate)
#define lw_editable_table_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_EDITABLE_TABLE, LwEditableTableClassPrivate)

static void lw_editable_table_get_begin_iter (LwEditableTable * self, LwIter * iter);
static void lw_editable_table_get_end_iter (LwEditableTable * self, LwIter * iter);
static gint lw_editable_table_get_n_columns (LwEditableTable * self);
static GType lw_editable_table_get_column_type (LwEditableTable * self, gint column);
static gboolean lw_editable_table_get_iter_at_position (LwEditableTable * self,  LwIter * iter, gint position);
static gpointer lw_editable_table_iter_get (LwEditableTable * self, gint column);
static void lw_editable_table_iter_set (LwEditableTable * self, gint column, gpointer data);
static gint lw_editable_table_get_length (LwEditableTable * self);
static void lw_editable_table_allocate (LwEditableTable * self, gint length);

G_DEFINE_ABSTRACT_TYPE (LwEditableTable, lw_editable_table, LW_TYPE_TABLE)

static void 
lw_editable_table_init (LwEditableTable *self)
{
}

static void
lw_editable_table_class_init (LwEditableTableClass * klass)
{
    LwTableClass * table_class = NULL;

    table_class = LW_TABLE_CLASS (klass);

    table_klass->get_begin_iter = lw_editable_table_get_begin_iter;
    table_klass->get_end_iter = lw_editable_table_get_end_iter;
    table_klass->get_n_columns = lw_editable_table_get_n_columns;
    table_klass->get_column_type = lw_editable_table_get_column_type;
    table_klass->get_iter_at_position = lw_editable_table_get_iter_at_position;
    table_klass->get_length = lw_editable_table_get_length;

    table_class->iter_get = lw_editable_table_iter_get;
    table_class->iter_set = lw_editable_table_iter_set;

    table_class->allocate = lw_editable_table_allocate;
    table_class->sort = lw_editable_table_sort;
}

void 
lw_editable_table_sort (LwEditableTable * self, gint column, LwSortDirection direction)

{
    // Sanity checks
    g_return_if_fail (LW_IS_EDITABLE_TABLE (self));
    g_return_if_fail (column > -1);
    g_return_if_fail (direction == LW_SORT_DIRECTION_ASCENDING || direction == LW_SORT_DIRECTION_DESCENDING);

    // Declarations
    LwEditableTableClass * klass = NULL;

    // Initializations
    klass = LW_EDITABLE_TABLE_GET_CLASS (self);

    klass->sort (self, column, direction);
}




static gboolean 
lw_editable_table_iter_is_valid (LwIter * self) { 
    g_return_val_if_fail (self != NULL, FALSE); 
 
    LwEditableTablePrivate * priv = NULL; 
    GSequence * sequence = NULL; 
 
    if (!LW_IS_EDITABLE_TABLE (self)) { 
        return FALSE; 
    } 
 
    priv = lw_editable_table_get_instance_private (self->iterable); 
    sequence = priv->sequence; 
 
    return sequence == g_sequence_iter_get_sequence (self->user_data1); 
} 
 
 
static void 
lw_editable_table_get_begin_iter (LwEditableTable * self, 
                           LwIter    * iter) 
{ 
    g_return_if_fail (LW_IS_EDITABLE_TABLE (self)); 
    g_return_if_fail (iter != NULL); 
 
    const LwEditableTablePrivate * priv = NULL; 
    GSequence * sequence = NULL; 
    GSequenceIter * sequence_iter = NULL; 
 
    priv = lw_editable_table_get_instance_private (self); 
    sequence = priv->sequence; 
    sequence_iter =  g_sequence_get_begin_iter (sequence); 
 
    memset(iter, 0, sizeof(LwIter)); 
    iter->iterable = self; 
    iter->user_data1 = sequence_iter; 
} 
 
static void 
lw_editable_table_get_end_iter (LwEditableTable     * self, 
                         LwIter * iter) 
{ 
    g_return_if_fail (LW_IS_EDITABLE_TABLE (self)); 
    g_return_if_fail (iter != NULL); 
 
    const LwEditableTablePrivate * priv = NULL; 
    GSequence * sequence = NULL; 
    GSequenceIter * sequence_iter = NULL; 
 
    priv = lw_editable_table_get_instance_private (self); 
    sequence = priv->sequence; 
    sequence_iter = g_sequence_get_end_iter (sequence); 
 
    memset(iter, 0, sizeof(LwIter)); 
    iter->iterable = self; 
    iter->user_data1 = sequence_iter; 
} 
 
static gint 
lw_editable_table_get_n_columns (LwEditableTable * self) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_TABLE (self), 0); 
 
    const LwEditableTablePrivate * priv = NULL; 
    LwDictionaryCache * dictionary_cache = NULL; 
    LwDictionary * dictionary = NULL; 
 
    priv = lw_editable_table_get_instance_private (self); 
    dictionary_cache = priv->dictionary_cache; 
    dictionary = priv->dictionary; 
 
    return lw_table_get_n_columns (LW_TABLE (dictionary)); 
} 
 
static GType 
lw_editable_table_get_column_type (LwEditableTable * self, 
                            gint        column) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_TABLE (self), G_TYPE_INVALID); 
    g_return_val_if_fail (column > -1, G_TYPE_INVALID); 
 
    const LwEditableTablePrivate * priv = NULL; 
    LwDictionaryCache * dictionary_cache = NULL; 
    LwDictionary * dictionary = NULL; 
 
    priv = lw_editable_table_get_instance_private (self); 
    dictionary_cache = priv->dictionary_cache; 
    dictionary = priv->dictionary; 
 
    return lw_table_get_column_type (LW_TABLE (dictionary), column); 
} 
 
static gboolean 
lw_editable_table_get_iter_at_position (LwEditableTable     * self, 
                                 LwIter * iter, 
                                 gint            position) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_TABLE (self), FALSE); 
    g_return_val_if_fail (iter != NULL, FALSE); 
    g_return_val_if_fail (position > -1, FALSE); 
 
    const LwEditableTablePrivate * priv = NULL; 
    GSequence * sequence = NULL; 
    GSequenceIter * sequence_iter = NULL; 
 
    priv = lw_editable_table_get_instance_private (self); 
    sequence = priv->sequence; 
    sequence_iter =  g_sequence_get_iter_at_pos (sequence, position); 
 
    memset(iter, 0, sizeof(LwIter)); 
    iter->iterable = self; 
    iter->user_data1 = sequence_iter; 
} 
 
static gint 
lw_editable_table_get_length (LwEditableTable * self) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_TABLE (self), 0); 
 
    const LwEditableTablePrivate * priv = NULL; 
    GSequence * sequence = NULL; 
 
    priv = lw_editable_table_get_instance_private (self); 
    sequence = priv->sequence; 
 
    return g_sequence_get_length (sequence); 
} 
 
static gint 
lw_editable_table_iter_get_position (LwIter * self) 
{ 
    g_return_val_if_fail (self != NULL, -1); 
 
    return g_sequence_iter_get_position (self->user_data1); 
} 
 
static void 
lw_editable_table_iter_get_value (LwIter * self, gint column, GValue * value) 
{ 
    g_return_if_fail (self != NULL); 
    g_return_if_fail (column < 0); 
    g_return_if_fail (value != NULL); 
 
    g_value_init (value, G_TYPE_POINTER); 
    g_value_set_pointer (value, g_sequence_get (self->user_data1)); 
} 
 
static gboolean 
lw_editable_table_iter_next (LwIter * self) 
{ 
    g_return_val_if_fail (self != NULL, FALSE); 
 
    return !g_sequence_iter_is_end (self->user_data1); 
} 
 
static gboolean 
lw_editable_table_iter_previous (LwIter * self) 
{ 
    g_return_val_if_fail (self != NULL, FALSE); 
 
    return !g_sequence_iter_is_begin (self->user_data1); 
} 

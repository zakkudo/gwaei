//!
//! @file editable_list.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include "gettext.h"

#include "editable-list.h"

#define lw_editable_list_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_EDITABLE_LIST, LwEditableListClass)
#define lw_editable_list_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_editable_list_get_class(self), LW_TYPE_EDITABLE_LIST, LwEditableListClassPrivate)
#define lw_editable_list_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_EDITABLE_LIST, LwEditableListClassPrivate)

G_DEFINE_ABSTRACT_TYPE (LwEditableList, lw_editable_list, LW_TYPE_LIST)

static void 
lw_editable_list_init (LwEditableList *self)
{
}

static void
lw_editable_list_class_init (LwEditableListClass * klass)
{
}

void 
lw_editable_list_sort (LwEditableList * self, gint column, LwSortDirection direction)

{
    // Sanity checks
    g_return_if_fail (LW_IS_EDITABLE_LIST (self));
    g_return_if_fail (column > -1);
    g_return_if_fail (direction == LW_SORT_DIRECTION_ASCENDING || direction == LW_SORT_DIRECTION_DESCENDING);

    // Declarations
    LwEditableListClass * klass = NULL;

    // Initializations
    klass = LW_EDITABLE_LIST_GET_CLASS (self);

    klass->sort (self, column, direction);
}




static gboolean 
lw_editable_list_iter_is_valid (LwIter * self) { 
    g_return_val_if_fail (self != NULL, FALSE); 
 
    TYPE##Private * priv = NULL; 
    GSequence * sequence = NULL; 
 
    if (!LW_IS_EDITABLE_LIST (self)) { 
        return FALSE; 
    } 
 
    priv = lw_editable_list_get_instance_private (self->iterable); 
    sequence = priv->sequence; 
 
    return sequence == g_sequence_iter_get_sequence (self->user_data1); 
} 
 
 
static void 
lw_editable_list_get_begin_iter (TYPE * self, 
                           LwIter    * iter) 
{ 
    g_return_if_fail (LW_IS_EDITABLE_LIST (self)); 
    g_return_if_fail (iter != NULL); 
 
    const TYPE##Private * priv = NULL; 
    GSequence * sequence = NULL; 
    GSequenceIter * sequence_iter = NULL; 
 
    priv = lw_editable_list_get_instance_private (self); 
    sequence = priv->sequence; 
    sequence_iter =  g_sequence_get_begin_iter (sequence); 
 
    memset(iter, 0, sizeof(LwIter)); 
    iter->iterable = self; 
    iter->user_data1 = sequence_iter; 
} 
 
static void 
lw_editable_list_get_end_iter (TYPE     * self, 
                         LwIter * iter) 
{ 
    g_return_if_fail (LW_IS_EDITABLE_LIST (self)); 
    g_return_if_fail (iter != NULL); 
 
    const TYPE##Private * priv = NULL; 
    GSequence * sequence = NULL; 
    GSequenceIter * sequence_iter = NULL; 
 
    priv = lw_editable_list_get_instance_private (self); 
    sequence = priv->sequence; 
    sequence_iter = g_sequence_get_end_iter (sequence); 
 
    memset(iter, 0, sizeof(LwIter)); 
    iter->iterable = self; 
    iter->user_data1 = sequence_iter; 
} 
 
static gint 
lw_editable_list_get_n_columns (TYPE * self) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_LIST (self), 0); 
 
    const TYPE##Private * priv = NULL; 
    LwDictionaryCache * dictionary_cache = NULL; 
    LwDictionary * dictionary = NULL; 
 
    priv = lw_editable_list_get_instance_private (self); 
    dictionary_cache = priv->dictionary_cache; 
    dictionary = priv->dictionary; 
 
    return lw_list_get_n_columns (LW_LIST (dictionary)); 
} 
 
static GType 
lw_editable_list_get_column_type (TYPE * self, 
                            gint        column) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_LIST (self), G_TYPE_INVALID); 
    g_return_val_if_fail (column > -1, G_TYPE_INVALID); 
 
    const TYPE##Private * priv = NULL; 
    LwDictionaryCache * dictionary_cache = NULL; 
    LwDictionary * dictionary = NULL; 
 
    priv = lw_editable_list_get_instance_private (self); 
    dictionary_cache = priv->dictionary_cache; 
    dictionary = priv->dictionary; 
 
    return lw_list_get_column_type (LW_LIST (dictionary), column); 
} 
 
static gboolean 
lw_editable_list_get_iter_at_position (TYPE     * self, 
                                 LwIter * iter, 
                                 gint            position) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_LIST (self), FALSE); 
    g_return_val_if_fail (iter != NULL, FALSE); 
    g_return_val_if_fail (position > -1, FALSE); 
 
    const TYPE##Private * priv = NULL; 
    GSequence * sequence = NULL; 
    GSequenceIter * sequence_iter = NULL; 
 
    priv = lw_editable_list_get_instance_private (self); 
    sequence = priv->sequence; 
    sequence_iter =  g_sequence_get_iter_at_pos (sequence, position); 
 
    memset(iter, 0, sizeof(LwIter)); 
    iter->iterable = self; 
    iter->user_data1 = sequence_iter; 
} 
 
static gint 
lw_editable_list_get_length (TYPE * self) 
{ 
    g_return_val_if_fail (LW_IS_EDITABLE_LIST (self), 0); 
 
    const TYPE##Private * priv = NULL; 
    GSequence * sequence = NULL; 
 
    priv = lw_editable_list_get_instance_private (self); 
    sequence = priv->sequence; 
 
    return g_sequence_get_length (sequence); 
} 
 
static gint 
lw_editable_list_iter_get_position (LwIter * self) 
{ 
    g_return_val_if_fail (self != NULL, -1); 
 
    return g_sequence_iter_get_position (self->user_data1); 
} 
 
static void 
lw_editable_list_iter_get_value (LwIter * self, gint column, GValue * value) 
{ 
    g_return_if_fail (self != NULL); 
    g_return_if_fail (column < 0); 
    g_return_if_fail (value != NULL); 
 
    g_value_init (value, G_TYPE_POINTER); 
    g_value_set_pointer (value, g_sequence_get (self->user_data1)); 
} 
 
static gboolean 
lw_editable_list_iter_next (LwIter * self) 
{ 
    g_return_val_if_fail (self != NULL, FALSE); 
 
    return !g_sequence_iter_is_end (self->user_data1); 
} 
 
static gboolean 
lw_editable_list_iter_previous (LwIter * self) 
{ 
    g_return_val_if_fail (self != NULL, FALSE); 
 
    return !g_sequence_iter_is_begin (self->user_data1); 
} 

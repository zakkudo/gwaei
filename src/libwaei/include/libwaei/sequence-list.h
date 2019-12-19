#ifndef LW_SEQUENCE_LIST_INCLUDED
#define LW_SEQUENCE_LIST_INCLUDED

G_BEGIN_DECLS

GHashTable * lw_sequence_list_previous_order_table_new (GSequence * sequence);
gint * lw_sequence_list_map_new_order (GSequence  * sequence,  GHashTable * previous_order_table);

#define LW_SEQUENCE_LIST_DEFINE_METHODS(SCOPE, TYPE, IS_TYPE) static gboolean \
SCOPE##_iter_is_valid (LwIter * self) { \
    g_return_val_if_fail (self != NULL, FALSE); \
 \
    TYPE##Private * priv = NULL; \
    GSequence * sequence = NULL; \
 \
    if (!IS_TYPE (self)) { \
        return FALSE; \
    } \
 \
    priv = SCOPE##_get_instance_private (self->iterable); \
    sequence = priv->sequence; \
 \
    return sequence == g_sequence_iter_get_sequence (self->user_data1); \
} \
 \
 \
static void \
SCOPE##_get_begin_iter (TYPE * self, \
                           LwIter    * iter) \
{ \
    g_return_if_fail (IS_TYPE (self)); \
    g_return_if_fail (iter != NULL); \
 \
    const TYPE##Private * priv = NULL; \
    GSequence * sequence = NULL; \
    GSequenceIter * sequence_iter = NULL; \
 \
    priv = SCOPE##_get_instance_private (self); \
    sequence = priv->sequence; \
    sequence_iter =  g_sequence_get_begin_iter (sequence); \
 \
    memset(iter, 0, sizeof(LwIter)); \
    iter->iterable = self; \
    iter->user_data1 = sequence_iter; \
} \
 \
static void \
SCOPE##_get_end_iter (TYPE     * self, \
                         LwIter * iter) \
{ \
    g_return_if_fail (IS_TYPE (self)); \
    g_return_if_fail (iter != NULL); \
 \
    const TYPE##Private * priv = NULL; \
    GSequence * sequence = NULL; \
    GSequenceIter * sequence_iter = NULL; \
 \
    priv = SCOPE##_get_instance_private (self); \
    sequence = priv->sequence; \
    sequence_iter = g_sequence_get_end_iter (sequence); \
 \
    memset(iter, 0, sizeof(LwIter)); \
    iter->iterable = self; \
    iter->user_data1 = sequence_iter; \
} \
 \
static gint \
SCOPE##_get_n_columns (TYPE * self) \
{ \
    g_return_val_if_fail (IS_TYPE (self), 0); \
 \
    const TYPE##Private * priv = NULL; \
    LwDictionaryCache * dictionary_cache = NULL; \
    LwDictionary * dictionary = NULL; \
 \
    priv = SCOPE##_get_instance_private (self); \
    dictionary_cache = priv->dictionary_cache; \
    dictionary = priv->dictionary; \
 \
    return lw_list_get_n_columns (LW_LIST (dictionary)); \
} \
 \
static GType \
SCOPE##_get_column_type (TYPE * self, \
                            gint        column) \
{ \
    g_return_val_if_fail (IS_TYPE (self), G_TYPE_INVALID); \
    g_return_val_if_fail (column > -1, G_TYPE_INVALID); \
 \
    const TYPE##Private * priv = NULL; \
    LwDictionaryCache * dictionary_cache = NULL; \
    LwDictionary * dictionary = NULL; \
 \
    priv = SCOPE##_get_instance_private (self); \
    dictionary_cache = priv->dictionary_cache; \
    dictionary = priv->dictionary; \
 \
    return lw_list_get_column_type (LW_LIST (dictionary), column); \
} \
 \
static gboolean \
SCOPE##_get_iter_at_position (TYPE     * self, \
                                 LwIter * iter, \
                                 gint            position) \
{ \
    g_return_val_if_fail (IS_TYPE (self), FALSE); \
    g_return_val_if_fail (iter != NULL, FALSE); \
    g_return_val_if_fail (position > -1, FALSE); \
 \
    const TYPE##Private * priv = NULL; \
    GSequence * sequence = NULL; \
    GSequenceIter * sequence_iter = NULL; \
 \
    priv = SCOPE##_get_instance_private (self); \
    sequence = priv->sequence; \
    sequence_iter =  g_sequence_get_iter_at_pos (sequence, position); \
 \
    memset(iter, 0, sizeof(LwIter)); \
    iter->iterable = self; \
    iter->user_data1 = sequence_iter; \
} \
 \
static gint \
SCOPE##_get_length (TYPE * self) \
{ \
    g_return_val_if_fail (IS_TYPE (self), 0); \
 \
    const TYPE##Private * priv = NULL; \
    GSequence * sequence = NULL; \
 \
    priv = SCOPE##_get_instance_private (self); \
    sequence = priv->sequence; \
 \
    return g_sequence_get_length (sequence); \
} \
 \
static gint \
SCOPE##_iter_get_position (LwIter * self) \
{ \
    g_return_val_if_fail (self != NULL, -1); \
 \
    return g_sequence_iter_get_position (self->user_data1); \
} \
 \
static void \
SCOPE##_iter_get_value (LwIter * self, gint column, GValue * value) \
{ \
    g_return_if_fail (self != NULL); \
    g_return_if_fail (column < 0); \
    g_return_if_fail (value != NULL); \
 \
    g_value_init (value, G_TYPE_POINTER); \
    g_value_set_pointer (value, g_sequence_get (self->user_data1)); \
} \
 \
static gboolean \
SCOPE##_iter_next (LwIter * self) \
{ \
    g_return_val_if_fail (self != NULL, FALSE); \
 \
    return !g_sequence_iter_is_end (self->user_data1); \
} \
 \
static gboolean \
SCOPE##_iter_previous (LwIter * self) \
{ \
    g_return_val_if_fail (self != NULL, FALSE); \
 \
    return !g_sequence_iter_is_begin (self->user_data1); \
} 

G_END_DECLS

#endif

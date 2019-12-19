#include <glib.h>


GHashTable *
lw_sequence_list_previous_order_table_new (GSequence * sequence) {
    GHashTable * table = NULL;
    GSequenceIter * iter = NULL;
    gint position = 0;

    table = g_hash_table_new (g_direct_hash, g_direct_equal);
    iter = g_sequence_get_begin_iter (sequence);

    while (!g_sequence_iter_is_end (iter)) {
        g_hash_table_insert (table, iter, GINT_TO_POINTER (position));
        position += 1;
        iter = g_sequence_iter_next (iter);
    }

    return table;
}

gint *
lw_sequence_list_map_new_order (GSequence  * sequence, 
                           GHashTable * previous_order_table)
{
    gint length = g_sequence_get_length (sequence);
    gint * new_order = NULL;
    GSequenceIter * iter = NULL;
    gint i = 0;

    new_order = g_new0 (gint, length);
    iter = g_sequence_get_begin_iter (sequence);

    while (!g_sequence_iter_is_end (iter)) {
        new_order[i] = GPOINTER_TO_INT (g_hash_table_lookup (previous_order_table, iter));
        iter = g_sequence_iter_next (iter);
    }

    return new_order;
}

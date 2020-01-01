//!
//! @file list.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include "gettext.h"

#include "list.h"

#define lw_list_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_LIST, LwListClass)
#define lw_list_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_list_get_class(self), LW_TYPE_LIST, LwListClassPrivate)
#define lw_list_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_LIST, LwListClassPrivate)


typedef enum {
  SIGNALID_ROW_CHANGED,
  SIGNALID_ROW_INSERTED,
  SIGNALID_ROW_DELETED,
  SIGNALID_ROWS_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

typedef void (* LwListRowChangedFunc) (LwList *list, gint index, LwIter * iter);
typedef void (* LwListRowInsertedFunc) (LwList *list, gint index, LwIter * iter);
typedef void (* LwListRowDeletedFunc) (LwList *list, gint index);
typedef void (* LwListRowsReorderedFunc) (LwList *list, gint index, LwIter * iter, gint * new_order);

typedef struct {
  guint signalid[TOTAL_SIGNALIDS];

  /* Signals */
  LwListRowChangedFunc row_changed;
  LwListRowInsertedFunc row_inserted;
  LwListRowDeletedFunc row_deleted;
  LwListRowsReorderedFunc rows_reordered;
} LwListClassPrivate;

G_DEFINE_ABSTRACT_TYPE (LwList, lw_list, G_TYPE_OBJECT)

static void 
lw_list_init (LwList *self)
{
}

static void
lw_list_finalize (GObject * object)
{

    G_OBJECT_CLASS (lw_dictionary_parent_class)->finalize (object);
}

static void
lw_list_class_init (LwListClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = lw_list_finalize;

    // Declarations
    LwListClassPrivate * klasspriv = NULL;
    
    // Initializations
    klasspriv = lw_list_class_get_private (klass);

    /**
     * LwResults::row-changed:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROW_CHANGED] = g_signal_new (
        "row-changed",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::row-inserted:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROW_INSERTED] = g_signal_new (
        "row-inserted",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::row-deleted:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROW_DELETED] = g_signal_new (
        "row-deleted",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::rows-reordered:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROWS_REORDERED] = g_signal_new (
        "rows-reordered",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );
}

void 
lw_list_get_begin_iter (LwList * self,
                            LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    klass->get_begin_iter (self, iter);
}

void 
lw_list_get_end_iter (LwList * self, 
                      LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    klass->get_end_iter (self, iter);
}

gboolean 
lw_list_get_iter_at_position (LwList * self, 
                                  LwIter     * iter, 
                                  gint         position)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS(self);

    return klass->get_iter_at_position (self, iter, position);
}


gint 
lw_list_get_length (LwList *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), 0);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS(self);

    return klass->get_length (self);
}


static GHashTable *
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

static gint *
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


void
lw_list_emit_row_inserted (LwList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_INSERTED], 0, index);
}

void
lw_list_emit_row_changed (LwList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_CHANGED], 0, index);
}

void
lw_list_emit_rows_reordered (LwList * self, gint * new_order)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROWS_REORDERED], 0, new_order);
}

void
lw_list_emit_row_deleted (LwList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_DELETED], 0, index);
}

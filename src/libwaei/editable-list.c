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

typedef enum {
  SIGNALID_ROW_CHANGED,
  SIGNALID_ROW_INSERTED,
  SIGNALID_ROW_DELETED,
  SIGNALID_ROWS_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

typedef void (* LwEditableListRowChangedFunc) (LwList *list, gint index, LwIter * iter);
typedef void (* LwEditableListRowInsertedFunc) (LwList *list, gint index, LwIter * iter);
typedef void (* LwEditableListRowDeletedFunc) (LwList *list, gint index);
typedef void (* LwEditableListRowsReorderedFunc) (LwList *list, gint index, LwIter * iter, gint * new_order);

typedef struct {
  guint signalid[TOTAL_SIGNALIDS];

  /* Signals */
  LwEditableListRowChangedFunc row_changed;
  LwEditableListRowInsertedFunc row_inserted;
  LwEditableListRowDeletedFunc row_deleted;
  LwEditableListRowsReorderedFunc rows_reordered;
} LwEditableListClassPrivate;

G_DEFINE_ABSTRACT_TYPE (LwEditableList, lw_editable_list, LW_TYPE_LIST)

static void 
lw_editable_list_init (LwEditableList *self)
{
}

static void
lw_editable_list_class_init (LwEditableListClass * klass)
{
    // Declarations
    LwEditableListClassPrivate * klasspriv = NULL;
    
    // Initializations
    klasspriv = lw_editable_list_class_get_private (klass);

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
        G_STRUCT_OFFSET (LwEditableListClassPrivate, row_changed),
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
        G_STRUCT_OFFSET (LwEditableListClassPrivate, row_inserted),
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
        G_STRUCT_OFFSET (LwEditableListClassPrivate, row_deleted),
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
        G_STRUCT_OFFSET (LwEditableListClassPrivate, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );
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


void
lw_editable_list_emit_row_inserted (LwEditableList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_EDITABLE_LIST (self));

    // Declarations
    LwEditableListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_editable_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_INSERTED], 0, index);
}

void
lw_editable_list_emit_row_changed (LwEditableList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_EDITABLE_LIST (self));

    // Declarations
    LwEditableListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_editable_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_CHANGED], 0, index);
}

void
lw_editable_list_emit_rows_reordered (LwEditableList * self, gint * new_order)
{
    // Sanity checks
    g_return_if_fail (LW_IS_EDITABLE_LIST (self));

    // Declarations
    LwEditableListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_editable_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROWS_REORDERED], 0, new_order);
}

void
lw_editable_list_emit_row_deleted (LwEditableList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_EDITABLE_LIST (self));

    // Declarations
    LwEditableListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_editable_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_DELETED], 0, index);
}

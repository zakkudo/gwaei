//!
//! @file iterable.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include <libwaei/gettext.h>

#include <libwaei/iterable.h>

typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_INSERTED,
  SIGNALID_DELETED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

typedef struct LwIterableClassPrivate {
  guint signalid[TOTAL_SIGNALIDS];
};

#define lw_iterable_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_ITERABLE, LwIterableClass)
#define lw_iterable_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_iterable_get_class(self), LW_TYPE_ITERABLE, LwIterableClassPrivate)
#define lw_iterable_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_ITERABLE, LwIterableClassPrivate)

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (LwIterable, lw_iterable, G_TYPE_OBJECT, g_type_add_class_private(LW_TYPE_ITERABLE, sizeof(LwIterableClassPrivate)))

static void 
lw_iterable_init (LwIterable *self)
{
}

static void
lw_iterable_class_init (LwIterableClass * klass)
{
    // Declarations
    LwIterableClassPrivate * klasspriv = NULL;
    
    // Initializations
    klasspriv = lw_iterable_class_get_private (klass);

    /**
     * LwResults::row-changed:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClassPrivate, row_changed),
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
    klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClassPrivate, row_inserted),
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
    klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClassPrivate, row_deleted),
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
    klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
        "rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwResultsClassPrivate, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );
}

void 
lw_iterable_get_begin_iter (LwIterable * self,
                            LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_ITERABLE (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwIterableClass * klass = NULL;

    // Initializations
    klass = LW_ITERABLE_GET_CLASS (self);

    klass->get_begin_iter (self, iter);
}

void 
lw_iterable_get_end_iter (LwIterable * self, 
                          LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_ITERABLE (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwIterableClass * klass = NULL;

    // Initializations
    klass = LW_ITERABLE_GET_CLASS (self);

    klass->get_end_iter (self, iter);
}

gint
lw_iterable_get_n_columns (LwIterable *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self), 0);

    // Declarations
    LwIterableClass * klass = NULL;

    // Initializations
    klass = LW_ITERABLE_GET_CLASS (self);

    return klass->get_n_columns (self);
}

GType 
lw_iterable_get_column_type (LwIterable * self,
                             gint         column)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self), G_TYPE_INVALID);

    // Declarations
    LwIterableClass * klass = NULL;

    // Initializations
    klass = LW_ITERABLE_GET_CLASS (self);

    return klass->get_column_type (self, column);
}

gboolean 
lw_iterable_get_iter_at_position (LwIterable * self, 
                                  LwIter     * iter, 
                                  gint         position)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    // Declarations
    LwIterableClass * klass = NULL;

    // Initializations
    klass = LW_ITERABLE_GET_CLASS(self);

    return klass->get_iter_at_position (self, iter, position);
}


gint 
lw_iterable_get_length (LwIterable *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self), 0);

    // Declarations
    LwIterableClass * klass = NULL;

    // Initializations
    klass = LW_ITERABLE_GET_CLASS(self);

    return klass->get_length (self);
}

gint
lw_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    // Declarations
    LwIterable * iterable = NULL;
    LwIterableClass * klass = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    klass = LW_ITERABLE_GET_CLASS (iterable);

    return klass->iter_get_position (self);
}

void 
lw_iter_get_value (LwIter * self, 
                   gint     column, 
                   GValue * value)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (value != NULL);

    // Declarations
    LwIterable * iterable = NULL;
    LwIterableClass * klass = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    klass = LW_ITERABLE_GET_CLASS (iterable);

    klass->iter_get_value (self, column, value);
}

gboolean 
lw_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwIterable * iterable = NULL;
    LwIterableClass * klass = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    klass = LW_ITERABLE_GET_CLASS (iterable);

    return klass->iter_next (self);
}

gboolean 
lw_iter_previous (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwIterable * iterable = NULL;
    LwIterableClass * klass = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    klass = LW_ITERABLE_GET_CLASS (iterable);

    return klass->iter_previous (self);
}

void
lw_iterable_emit_row_inserted (LwIterable * self, gint index)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self));

    // Declarations
    LwIterableClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_iterable_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED], 0, index);
}

void
lw_iterable_emit_row_changed (LwIterable * self, gint index)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self));

    // Declarations
    LwIterableClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_iterable_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED], 0, index);
}

void
lw_iterable_emit_rows_reordered (LwIterable * self, gint * new_order)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self));

    // Declarations
    LwIterableClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_iterable_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED], 0, new_order);
}

void
lw_iterable_emit_row_deleted (LwIterable * self, gint index)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self));

    // Declarations
    LwIterableClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_iterable_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED], 0, index);
}

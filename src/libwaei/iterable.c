//!
//! @file iterable.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include <libwaei/gettext.h>

#include <libwaei/iterable.h>

G_DEFINE_INTERFACE (LwIterable, lw_iterable, G_TYPE_OBJECT)

static void
lw_iterable_default_init (LwIterableInterface *iface)
{
    /*
    iface->get_begin_iter(LwIterable *iterable, LwIter * iter);
    iface->get_end_iter(LwIterable *iterable, LwIter * iter);
    iface->get_n_columns (LwIterable *iterable);
    iface->get_column_type (LwIterable *iterable, gint column);
    iface->get_iter_at_pos (LwIterable *iterable, LwIter *iter, gint position);
    iface->iter_get_position (LwIterable *iterable, LwIter  *iter);
    iface->get_value (LwIter *iter, gint column, GValue *value);
    iface->iter_next (LwIter *iter);
    iface->iter_previous (LwIterable *iterable, LwIter  *iter);
    iface->get_length (LwIterable *iterable);
    */
}

void 
lw_iterable_get_begin_iter (LwIterable * self,
                            LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_ITERABLE (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwIterableInterface * iface = NULL;

    // Initializations
    iface = LW_ITERABLE_GET_IFACE (self);

    iface->get_begin_iter (self, iter);
}

void 
lw_iterable_get_end_iter (LwIterable * self, 
                          LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_ITERABLE (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwIterableInterface * iface = NULL;

    // Initializations
    iface = LW_ITERABLE_GET_IFACE (self);

    iface->get_end_iter (self, iter);
}

gint
lw_iterable_get_n_columns (LwIterable *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self), 0);

    // Declarations
    LwIterableInterface * iface = NULL;

    // Initializations
    iface = LW_ITERABLE_GET_IFACE (self);

    return iface->get_n_columns (self);
}

GType 
lw_iterable_get_column_type (LwIterable * self,
                             gint         column)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self), G_TYPE_INVALID);

    // Declarations
    LwIterableInterface * iface = NULL;

    // Initializations
    iface = LW_ITERABLE_GET_IFACE (self);

    return iface->get_column_type (self, column);
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
    LwIterableInterface * iface = NULL;

    // Initializations
    iface = LW_ITERABLE_GET_IFACE(self);

    return iface->get_iter_at_position (self, iter, position);
}


gint 
lw_iterable_get_length (LwIterable *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_ITERABLE (self), 0);

    // Declarations
    LwIterableInterface * iface = NULL;

    // Initializations
    iface = LW_ITERABLE_GET_IFACE(self);

    return iface->get_length (self);
}

gint
lw_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    // Declarations
    LwIterable * iterable = NULL;
    LwIterableInterface * iface = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    iface = LW_ITERABLE_GET_IFACE (iterable);

    return iface->iter_get_position (self);
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
    LwIterableInterface * iface = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    iface = LW_ITERABLE_GET_IFACE (iterable);

    iface->iter_get_value (self, column, value);
}

gboolean 
lw_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwIterable * iterable = NULL;
    LwIterableInterface * iface = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    iface = LW_ITERABLE_GET_IFACE (iterable);

    return iface->iter_next (self);
}

gboolean 
lw_iter_previous (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwIterable * iterable = NULL;
    LwIterableInterface * iface = NULL;

    // Initializations
    iterable = LW_ITERABLE (self->iterable);
    iface = LW_ITERABLE_GET_IFACE (iterable);

    return iface->iter_previous (self);
}

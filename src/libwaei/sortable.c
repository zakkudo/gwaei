//!
//! @file sortable.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include <libwaei/gettext.h>

#include <libwaei/sortable.h>

G_DEFINE_ABSTRACT_TYPE (LwSortable, lw_sortable, LW_TYPE_ITERABLE)

static void 
lw_sortable_init (LwSortable *self)
{
}

static void
lw_sortable_class_init (LwSortableClass * klass)
{
}

void 
lw_sortable_sort (LwSortable * self, gint column, LwSortDirection direction)

{
    // Sanity checks
    g_return_if_fail (LW_IS_SORTABLE (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwSortableClass * klass = NULL;

    // Initializations
    klass = LW_SORTABLE_GET_CLASS (self);

    klass->sort (self, column, direction);
}

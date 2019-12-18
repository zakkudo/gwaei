//!
//! @file list.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include <libwaei/gettext.h>

#include <libwaei/list.h>

#define lw_list_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_LIST, LwListClass)
#define lw_list_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_list_get_class(self), LW_TYPE_LIST, LwListClassPrivate)
#define lw_list_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_LIST, LwListClassPrivate)

G_DEFINE_ABSTRACT_TYPE (LwList, lw_list, G_TYPE_OBJECT)

static void 
lw_list_init (LwList *self)
{
}

static void
lw_list_class_init (LwListClass * klass)
{
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

gint
lw_list_get_n_columns (LwList *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), 0);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    return klass->get_n_columns (self);
}

GType 
lw_list_get_column_type (LwList * self,
                             gint         column)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), G_TYPE_INVALID);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    return klass->get_column_type (self, column);
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

gint
lw_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

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
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    klass->iter_get_value (self, column, value);
}

gboolean 
lw_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_next (self);
}

gboolean 
lw_iter_previous (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_previous (self);
}

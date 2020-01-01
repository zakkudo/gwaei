#include <glib.h>
#include "iter.h"
#include "list.h"
#include "table.h"

// List methods 

gint
lw_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_get_position (self);
}

gboolean 
lw_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);

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
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_previous (self);
}

gboolean 
lw_iter_is_end (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_is_end (self);
}

gboolean 
lw_iter_is_begin (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_is_begin (self);
}

gpointer
lw_iter_get (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (LW_IS_LIST (self->iterable), NULL);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_get (self);
}


void
lw_iter_set (LwIter * self, 
             gpointer data)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (LW_IS_LIST (self->iterable));

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->iterable);
    klass = LW_LIST_GET_CLASS (list);

    klass->iter_set (self, data);
}


static void
copy(gpointer destination, gpointer source, GType type)
{
    switch (type) {
        case G_TYPE_INT:
            memcpy(destination, source, sizeof(gint));
            break;
        case G_TYPE_UINT:
            memcpy(destination, source, sizeof(guint));
            break;
        case G_TYPE_LONG:
            memcpy(destination, source, sizeof(glong));
            break;
        case G_TYPE_ULONG:
            memcpy(destination, source, sizeof(gulong));
            break;
        case G_TYPE_CHAR:
            memcpy(destination, source, sizeof(gchar));
            break;
        case G_TYPE_UCHAR:
            memcpy(destination, source, sizeof(guchar));
            break;
        case G_TYPE_FLOAT:
            memcpy(destination, source, sizeof(gfloat));
            break;
        case G_TYPE_DOUBLE:
            memcpy(destination, source, sizeof(gdouble));
            break;
        case G_TYPE_STRING:
            memcpy(destination, source, sizeof(gchar*));
            break;
        case G_TYPE_POINTER:
        case G_TYPE_BOXED:
        case G_TYPE_OBJECT:
            memcpy(destination, source, sizeof(gpointer));
            break;
        default:
            g_assert_not_reached ();
    }
}

// Table methods

void
lw_iter_get_columns_valist (LwIter  * self,
                            va_list   va)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (LW_IS_TABLE (self));

    // Declarations
    LwTable * table = NULL;
    LwTableClass * klass = NULL;
    gint i = 0;
    gint column;
    GType type = G_TYPE_INVALID;
    gpointer data = NULL;
    gpointer destination = NULL;

    while ((column = va_arg (va, gint)) != -1)
    {
        klass = LW_TABLE_GET_CLASS (table);
        data = klass->iter_get_column (self, column);
        table = LW_TABLE (self->iterable);
        type = G_TYPE_FUNDAMENTAL (lw_table_get_column_type (table, column));
        i += 1;

        destination = va_arg (va, gpointer);
        copy (destination, data, type);
        i += 1;
    }
} 

void
lw_iter_get_columns (LwIter * self,
             ...)
{
    // Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    va_list va;

    //Initializations
    va_start(va, self);

    lw_iter_get_columns_valist (self, va);

    va_end (va);
}

void
lw_iter_set_columns_valist (LwIter  * self,
                            va_list   va)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (LW_IS_TABLE (self->iterable));

    // Declarations
    LwTable * table = NULL;
    gint i = 0;
    gint column = 0;
    GType type = G_TYPE_INVALID;
    gpointer data = NULL;
    gpointer destination = NULL;

    // Initializations
    table = LW_TABLE (self->iterable);

    while ((column = va_arg (va, gint)) != -1)
    {
        type = G_TYPE_FUNDAMENTAL (lw_table_get_column_type (table, column));
        i += 1;

        data = va_arg (va, gpointer);
        copy (destination, data, type);
        i += 1;
    }
} 

void
lw_iter_set_columns (LwIter * self,
             ...)
{
    // Sanity checks
    g_return_if_fail (self != NULL);

    // Declarations
    va_list va;

    // Initializations
    va_start(va, self);

    lw_iter_set_columns_valist (self, va);

    va_end (va);
}


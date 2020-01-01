#ifndef LW_ITER_INCLUDED
#define LW_ITER_INCLUDED

#include "glib.h"
#include "glib-object.h"

G_BEGIN_DECLS

typedef struct {
    gpointer iterable;
    gpointer user_data1;
    gpointer user_data2;
} LwIter;

// List
gint lw_iter_get_position (LwIter  *self);
gboolean lw_iter_next (LwIter *self);
gboolean lw_iter_previous (LwIter  *self);
gboolean lw_iter_is_end (LwIter * self);
gboolean lw_iter_is_begin (LwIter * self);

gpointer lw_iter_get (LwIter * self);
void lw_iter_set (LwIter * self, gpointer data);

// Table
gpointer lw_iter_get_column (LwIter * self, gint column);
void lw_iter_get_columns_valist (LwIter * self, va_list va);
void lw_iter_get_columns (LwIter * self, ...);

void lw_iter_set_column (LwIter * self, gint column, gpointer data);
void lw_iter_set_columns_valist (LwIter * self, va_list va);
void lw_iter_set_columns (LwIter * self, ...);

G_END_DECLS

#endif

#ifndef LW_ITER_INCLUDED
#define LW_ITER_INCLUDED

G_BEGIN_DECLS

typedef struct {
    gpointer iterable;
    gpointer user_data1;
    gpointer user_data2;
} LwIter;

gint lw_iter_get_position (LwIter  *self);
void lw_iter_get_value (LwIter *self, gint column, GValue *value);
gboolean lw_iter_next (LwIter *self);
gboolean lw_iter_previous (LwIter  *self);

void lw_iter_get_value (LwIter * self, gint column, GValue * value);
void lw_iter_get_valist (LwIter * self, va_list * va);
void lw_iter_get (LwIter * self, ...);

void lw_iter_set_value (LwIter * self, gint column, GValue * value);
void lw_iter_set_valist (LwIter *self, va_list * va);
void lw_iter_set (LwIter * self, ...);

G_END_DECLS

#endif

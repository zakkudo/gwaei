#ifndef LW_ITERABLE_INCLUDED
#define LW_ITERABLE_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_ITERABLE lw_iterable_get_type()
G_DECLARE_DERIVABLE_TYPE (LwIterable, lw_iterable, LW, ITERABLE, GObject)

typedef struct {
    LwIterable * iterable;
    gpointer user_data1;
    gpointer user_data2;
    gpointer user_data3;
} LwIter;

typedef void (*LwIterableGetBeginIterFunc) (LwIterable *self, LwIter * iter);
typedef void (*LwIterableGetEndIterFunc)(LwIterable *self, LwIter * iter);
typedef gint (*LwIterableGetNColumnsFunc ) (LwIterable *self);
typedef GType (*LwIterableGetColumnTypeFunc) (LwIterable *self, gint column);
typedef gboolean (*LwIterableGetIterAtPositionFunc) (LwIterable *self, LwIter *iter, gint position);
typedef gint (*LwIterableGetLengthFunc) (LwIterable *self);

typedef gint (*LwIterableIterGetPositionFunc) (LwIter  *self);
typedef void (*LwIterableIterGetValueFunc) (LwIter *self, gint column, GValue *value);
typedef gboolean (*LwIterableIterNextFunc) (LwIter *self);
typedef gboolean (*LwIterableIterPreviousFunc) (LwIter  *self);

struct _LwIterableClass {
  GObjectClass parent_class;

  /* Signals */
  void (* row_changed)           (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_inserted)          (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_deleted)           (LwIterable *iterable, gint index);
  void (* rows_reordered)        (LwIterable *iterable, gint index, LwIter * iter, gint * new_order);

  /* Virtual methods */
  LwIterableGetBeginIterFunc get_begin_iter;
  LwIterableGetEndIterFunc get_end_iter;
  LwIterableGetNColumnsFunc get_n_columns;
  LwIterableGetColumnTypeFunc get_column_type;
  LwIterableGetIterAtPositionFunc get_iter_at_position;
  LwIterableGetLengthFunc get_length;

  LwIterableIterGetPositionFunc iter_get_position;
  LwIterableIterGetValueFunc iter_get_value;
  LwIterableIterNextFunc iter_next;
  LwIterableIterPreviousFunc iter_previous;
};

//Methods
void lw_iterable_get_begin_iter(LwIterable *self, LwIter * iter);
void lw_iterable_get_end_iter(LwIterable *self, LwIter * iter);
gint lw_iterable_get_n_columns (LwIterable *self);
GType lw_iterable_get_column_type (LwIterable *self, gint column);
gboolean lw_iterable_get_iter_at_pos (LwIterable *self, LwIter *iter, gint position);
gint lw_iterable_get_length (LwIterable *self);

gint lw_iter_get_position (LwIter  *self);
void lw_iter_get_value (LwIter *self, gint column, GValue *value);
gboolean lw_iter_next (LwIter *self);
gboolean lw_iter_previous (LwIter  *self);

void lw_iterable_emit_row_inserted (LwIterable * iterable, gint index);
void lw_iterable_emit_row_changed (LwIterable * iterable, gint index);
void lw_iterable_emit_rows_reordered (LwIterable * self, gint * new_order);
void lw_iterable_emit_row_deleted (LwIterable * self, gint index);

G_END_DECLS

#endif

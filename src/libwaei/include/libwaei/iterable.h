#ifndef LW_ITERABLE_INCLUDED
#define LW_ITERABLE_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwIterableInterface LwIterableInterface;
typedef struct _LwIter LwIter;

#define LW_TYPE_ITERABLE              (lw_iterable_get_type())
G_DECLARE_INTERFACE (LwIterable, lw_iterable, LW, ITERABLE, GObject)

struct _LwIter {
    LwIterable * iterable;
    gpointer user_data1;
    gpointer user_data2;
    gpointer user_data3;
};

struct _LwIterableInterface {
  GTypeInterface parent_iface;

  /* Signals */
  void (* row_changed)           (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_inserted)          (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_has_child_toggled) (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_deleted)           (LwIterable *iterable, gint index);
  void (* rows_reordered)        (LwIterable *iterable, gint index, LwIter * iter, gint * new_order);

  /* Virtual methods */
  void (*get_begin_iter) (LwIterable *self, LwIter * iter);
  void (*get_end_iter)(LwIterable *self, LwIter * iter);
  gint (*get_n_columns) (LwIterable *self);
  GType (*get_column_type) (LwIterable *self, gint column);
  gboolean (*get_iter_at_position) (LwIterable *self, LwIter *iter, gint position);
  gint (*get_length) (LwIterable *self);

  gint (*iter_get_position) (LwIter  *self);
  void (*iter_get_value) (LwIter *self, gint column, GValue *value);
  gboolean (*iter_next) (LwIter *self);
  gboolean (*iter_previous) (LwIter  *self);
};

//Methods
GType lw_iterable_get_type (void) G_GNUC_CONST;

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

G_END_DECLS

#endif

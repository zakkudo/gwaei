#ifndef LW_ITERABLE_INCLUDED
#define LW_ITERABLE_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwIterable LwIterable;
typedef struct _LwIterableClass LwIterableClass;
typedef struct _LwIter LwIter;

#define LW_TYPE_ITERABLE              (lw_dictionary_get_type())
#define LW_ITERABLE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_ITERABLE, LwIterable))
#define LW_ITERABLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_ITERABLE, LwIterableClass))
#define LW_IS_ITERABLE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_ITERABLE))
#define LW_IS_ITERABLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_ITERABLE))
#define LW_ITERABLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_ITERABLE, LwIterableClass))


#define VIEWER_TYPE_EDITABLE viewer_editable_get_type ()
G_DECLARE_INTERFACE (LwIterable, lw_iterable, LW, ITERABLE, GObject)

struct _LwIter {
    gint stamp;
    gpointer user_data1;
    gpointer user_data2;
    gpointer user_data3;
};

struct _LwIterable {
  GTypeInterface parent_iface;

  /* Signals */
  void (* row_changed)           (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_inserted)          (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_has_child_toggled) (LwIterable *iterable, gint index, LwIter * iter);
  void (* row_deleted)           (LwIterable *iterable, gint index);
  void (* rows_reordered)        (LwIterable *iterable, gint index, LwIter * iter, gint * new_order);

  /* Virtual methods */
  void (*get_begin_iter) (LwIterable *iterable, LwIter * iter);
  void (*get_end_iter)(LwIterable *iterable, LwIter * iter);
  gint (*get_n_columns) (LwIterable *iterable);
  GType (*get_column_type) (LwIterable *iterable, gint column);
  gboolean (*get_iter_at_pos) (LwIterable *iterable, LwIter *iter, gint position);
  gint (*get_length) (LwIterable *iterable);

  gint (*iter_get_position) (LwIter  *iter);
  void (*iter_get_value) (LwIter *iter, gint column, GValue *value);
  gboolean (*iter_next) (LwIter *iter);
  gboolean (*iter_previous) (LwIter  *iter);
};

//Methods
LwIterable * lw_dictionary_new (GType type, gchar const * FILENAME);
GType lw_dictionary_get_type (void) G_GNUC_CONST;

void lw_iterable_get_begin_iter(LwIterable *iterable, LwIter * iter);
void lw_iterable_get_end_iter(LwIterable *iterable, LwIter * iter);
gint lw_iterable_get_n_columns (LwIterable *iterable);
GType lw_iterable_get_column_type (LwIterable *iterable, gint column);
gboolean lw_iterable_get_iter_at_pos (LwIterable *iterable, LwIter *iter, gint position);
gint lw_iterable_get_length (LwIterable *iterable);

gint lw_iter_get_position (LwIter  *iter);
void lw_iter_get_value (LwIter *iter, gint column, GValue *value);
gboolean lw_iter_next (LwIter *iter);
gboolean lw_iter_previous (LwIter  *iter);

G_END_DECLS

#endif

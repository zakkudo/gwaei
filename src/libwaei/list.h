#ifndef LW_LIST_INCLUDED
#define LW_LIST_INCLUDED

#include <glib-object.h>
#include "iter.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_LIST lw_list_get_type()
G_DECLARE_DERIVABLE_TYPE (LwList, lw_list, LW, LIST, GObject)

typedef void (*LwListGetBeginIterFunc) (LwList *self, LwIter * iter);
typedef void (*LwListGetEndIterFunc)(LwList *self, LwIter * iter);
typedef gint (*LwListGetNColumnsFunc ) (LwList *self);
typedef GType (*LwListGetColumnTypeFunc) (LwList *self, gint column);
typedef gboolean (*LwListGetIterAtPositionFunc) (LwList *self, LwIter *iter, gint position);
typedef gint (*LwListGetLengthFunc) (LwList *self);

typedef gint (*LwListIterGetPositionFunc) (LwIter * self);
typedef void (*LwListIterGetValueFunc) (LwIter * self, gint column, GValue * value);
typedef gboolean (*LwListIterNextFunc) (LwIter * self);
typedef gboolean (*LwListIterPreviousFunc) (LwIter  *self);

typedef void (*LwListAllocateFunc) (LwList * self);

struct _LwListClass {
  GObjectClass parent_class;

  /* Virtual methods */
  LwListGetBeginIterFunc get_begin_iter;
  LwListGetEndIterFunc get_end_iter;
  LwListGetNColumnsFunc get_n_columns;
  LwListGetColumnTypeFunc get_column_type;
  LwListGetIterAtPositionFunc get_iter_at_position;
  LwListGetLengthFunc get_length;

  LwListIterGetPositionFunc iter_get_position;
  LwListIterGetValueFunc iter_get_value;
  LwListIterNextFunc iter_next;
  LwListIterPreviousFunc iter_previous;

  LwListAllocateFunc allocate;
};

//Methods
void lw_list_get_begin_iter(LwList *self, LwIter * iter);
void lw_list_get_end_iter(LwList *self, LwIter * iter);
gint lw_list_get_n_columns (LwList *self);
GType lw_list_get_column_type (LwList *self, gint column);
gboolean lw_list_get_iter_at_pos (LwList *self, LwIter *iter, gint position);
gint lw_list_get_length (LwList *self);

G_END_DECLS

#endif



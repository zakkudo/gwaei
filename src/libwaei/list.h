#ifndef LW_LIST_INCLUDED
#define LW_LIST_INCLUDED

#include <glib-object.h>
#include "iter.h"
#include "sort-direction.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_LIST lw_list_get_type()
G_DECLARE_DERIVABLE_TYPE (LwList, lw_list, LW, LIST, GObject)

typedef void (*LwListGetBeginIterFunc) (LwList *self, LwIter * iter);
typedef void (*LwListGetEndIterFunc)(LwList *self, LwIter * iter);
typedef gboolean (*LwListGetIterAtPositionFunc) (LwList *self, LwIter *iter, gint position);
typedef gint (*LwListGetLengthFunc) (LwList *self);

typedef gint (*LwListIterGetPositionFunc) (LwIter * self);
typedef gboolean (*LwListIterNextFunc) (LwIter * self);
typedef gboolean (*LwListIterPreviousFunc) (LwIter  *self);
typedef gpointer (*LwListIterGetFunc), (LwIter * self);
typedef void (*LwListIterSetFunc), (LwIter * self, gointer data);

typedef void (*LwListAllocateFunc) (LwList * self);

struct _LwListClass {
  GObjectClass parent_class;

  /* Virtual methods */
  LwListGetBeginIterFunc get_begin_iter;
  LwListGetEndIterFunc get_end_iter;
  LwListGetIterAtPositionFunc get_iter_at_position;
  LwListGetLengthFunc get_length;

  LwListIterGetPositionFunc iter_get_position;
  LwListIterNextFunc iter_next;
  LwListIterPreviousFunc iter_previous;

  LwListIterGetFunc iter_get;
  LwListIterSetFunc iter_set;

  LwListSortFunc sort;

  LwListAllocateFunc allocate;
};

//Methods
void lw_list_get_begin_iter (LwList *self, LwIter * iter);
void lw_list_get_end_iter (LwList *self, LwIter * iter);
gboolean lw_list_get_iter_at_position (LwList *self, LwIter *iter, gint position);
gint lw_list_get_length (LwList *self);

void lw_list_emit_row_inserted (LwList * list, gint index);
void lw_list_emit_row_changed (LwList * list, gint index);
void lw_list_emit_rows_reordered (LwList * self, gint * new_order);
void lw_list_emit_row_deleted (LwList * self, gint index);


G_END_DECLS

#endif



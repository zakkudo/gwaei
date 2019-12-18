#ifndef LW_SORTABLE_INCLUDED
#define LW_SORTABLE_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_SORTABLE lw_sortable_get_type()
G_DECLARE_DERIVABLE_TYPE (LwSortable, lw_sortable, LW, SORTABLE, LwIterable)

typedef void (*LwSortableSortFunc) (LwSortable *self, gint column, LwSortDirection direction);

struct _LwSortableClass {
  LwIterableClass parent_class;

  // Virtual methods
  LwSortableSortFunc sort;
};

//Methods
void lw_sortable_sort (LwSortable *self, gint column, LwSortDirection direction);

G_END_DECLS

#endif

#ifndef LW_EDITABLE_LIST_INCLUDED
#define LW_EDITABLE_LIST_INCLUDED

#include "sort-direction.h"
#include "list.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_EDITABLE_LIST lw_editable_list_get_type()
G_DECLARE_DERIVABLE_TYPE (LwEditableList, lw_editable_list, LW, EDITABLE_LIST, LwList)

typedef void (*LwEditableListSortFunc) (LwEditableList *self, gint column, LwSortDirection direction);

struct _LwEditableListClass {
  LwListClass parent_class;

  // Virtual methods
  LwEditableListSortFunc sort;
};

//Methods
void lw_editable_list_sort (LwEditableList *self, gint column, LwSortDirection direction);

void lw_editable_list_emit_row_inserted (LwEditableList * list, gint index);
void lw_editable_list_emit_row_changed (LwEditableList * list, gint index);
void lw_editable_list_emit_rows_reordered (LwEditableList * self, gint * new_order);
void lw_editable_list_emit_row_deleted (LwEditableList * self, gint index);

G_END_DECLS

#endif

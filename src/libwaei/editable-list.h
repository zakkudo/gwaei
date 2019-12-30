#ifndef LW_EDITABLE_LIST_INCLUDED
#define LW_EDITABLE_LIST_INCLUDED

#include "list.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_EDITABLE_LIST lw_editable_list_get_type()
G_DECLARE_DERIVABLE_TYPE (LwEditableList, lw_editable_list, LW, EDITABLE_LIST, LwList)

struct _LwEditableListClass {
  LwListClass parent_class;
};

G_END_DECLS

#endif

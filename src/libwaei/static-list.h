#ifndef LW_STATIC_LIST_INCLUDED
#define LW_STATIC_LIST_INCLUDED

#include <glib-object.h>
#include "iter.h"
#include "list.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_STATIC_LIST lw_static_list_get_type()
G_DECLARE_DERIVABLE_TYPE (LwStaticList, lw_static_list, LW, STATIC_LIST, LwList)

typedef gint (*LwStaticListGetNColumnsFunc ) (LwStaticList *self);
typedef GType (*LwStaticListGetColumnTypeFunc) (LwStaticList *self, gint column);

struct _LwStaticListClass {
  LwListClass parent_class;

  LwStaticListGetNColumnsFunc get_n_columns;
  LwStaticListGetColumnTypeFunc get_column_type;
};

G_END_DECLS

#endif



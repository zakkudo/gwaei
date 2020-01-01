#ifndef LW_STATIC_TABLE_INCLUDED
#define LW_STATIC_TABLE_INCLUDED

#include <glib-object.h>
#include "iter.h"
#include "table.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_STATIC_TABLE lw_static_table_get_type()
G_DECLARE_DERIVABLE_TYPE (LwStaticTable, lw_static_table, LW, STATIC_TABLE, LwTable)

struct _LwStaticTableClass {
  LwTableClass parent_class;
};

G_END_DECLS

#endif



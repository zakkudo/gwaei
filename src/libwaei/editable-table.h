#ifndef LW_EDITABLE_TABLE_INCLUDED
#define LW_EDITABLE_TABLE_INCLUDED

#include "table.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_EDITABLE_TABLE lw_editable_table_get_type()
G_DECLARE_DERIVABLE_TYPE (LwEditableTable, lw_editable_table, LW, EDITABLE_TABLE, LwTable)

struct _LwEditableTableClass {
  LwTableClass parent_class;
};

G_END_DECLS

#endif

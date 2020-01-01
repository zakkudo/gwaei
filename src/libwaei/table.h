#ifndef LW_TABLE_INCLUDED
#define LW_TABLE_INCLUDED

#include <glib-object.h>
#include "iter.h"
#include "sort-direction.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_TABLE lw_table_get_type()
G_DECLARE_DERIVABLE_TYPE (LwTable, lw_table, LW, TABLE, GObject)

typedef gint (*LwTableGetNColumnsFunc ) (LwTable *self);
typedef GType (*LwTableGetColumnTypeFunc) (LwTable *self, gint column);
typedef void (*LwTableIterGetValueFunc) (LwIter * self, gint column, GValue * value);
typedef gpointer (*LwTableIterGetFunc) (LwIter * self, gint column);
typedef void (*LwTableIterSetFunc) (LwIter * self, gint column, gpointer data);
typedef void (*LwTableSortFunc) (LwTable * self, gint column, LwSortDirection direction);

struct _LwTableClass {
  LwListClass parent_class;

  /* Virtual methods */
  LwTableGetColumnTypeFunc get_column_value_type;
  LwTableGetColumnTypeFunc get_column_type;
  LwTableIterGetValueFunc iter_get_value;
  LwTableIterGetFunc iter_get;
  LwTableIterSetFunc iter_set;

  LwTableSortFunc sort;
};

//Methods
gint lw_table_get_n_columns (LwTable *self);
GType lw_table_get_column_type (LwTable *self, gint column);
GType lw_table_get_column_value_type (LwTable *self, gint column);
gchar const * lw_table_get_column_nick (LwTable *self, gint column);
gchar const * lw_table_get_column_name (LwTable *self, gint column);
void lw_table_sort (LwTable *self, gint column, LwSortDirection direction);


G_END_DECLS

#endif



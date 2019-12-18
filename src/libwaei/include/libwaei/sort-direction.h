#ifndef LW_SORT_DIRECTION_INCLUDED
#define LW_SORT_DIRECTION_INCLUDED

#include "glib-object.h"

G_BEGIN_DECLS

typedef enum {
  LW_SORT_DIRECTION_ASCENDING, //!< Column is not indexed, but it is still searched
  LW_SORT_DIRECTION_DESCENDING, //!< Key is indexed and is included by default for all searches
  TOTAL_SORT_DIRECTION
} LwSortDirection;
GType lw_sort_direction_get_type (void);
#define LW_TYPE_SORT_DIRECTION (lw_sort_direction_get_type ())

#define LW_SORT_DIRECTION_NAME_ASCENDING "Ascending"
#define LW_SORT_DIRECTION_NAME_DESCENDING "Descending"

#define LW_SORT_DIRECTION_NICK_ASCENDING "ascending"
#define LW_SORT_DIRECTION_NICK_DESCENDING "descending"

G_END_DECLS

#endif

#ifndef LW_MATCH_INFO_INCLUDED
#define LW_MATCH_INFO_INCLUDED

#include "list.h"
#include "table.h"

G_BEGIN_DECLS

typedef struct {
    LwMatchInfo * match_info;
    GType type;
    GValue value;
    gint index;
    gint length;
    gboolean is_match;
    /*< private >*/
    gint match_level;
    GList * marker;
    gchar const * END;
} LwMatchInfoReadIter;

gboolean lw_match_info_read_iter_next (LwMatchInfoReadIter * iter, gint index);

G_END_DECLS

#endif

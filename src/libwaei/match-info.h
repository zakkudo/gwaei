#ifndef LW_MATCH_INFO_INCLUDED
#define LW_MATCH_INFO_INCLUDED

#include "column-match-info.h"
#include "match-marker.h"

G_BEGIN_DECLS

#define LW_TYPE_MATCH_INFO lw_match_info_get_type ()
G_DECLARE_FINAL_TYPE (LwMatchInfo, lw_match_info, LW, MATCH_INFO, LwList)

LwMatchInfo * lw_match_info_new (void);

void lw_match_info_set_column (LwMatchInfo * self, LwColumnMatchInfo * column_match_info);

gboolean lw_match_info_match_value (LwQueryNode * self, GValue * value LwMatchInfo * match_info_out);
gboolean lw_match_info_match_iter (LwQueryNode  * self, LwIter * iter, LwMatchInfo * match_info_out);

G_END_DECLS

#endif

#ifndef LW_MATCH_INFO_INCLUDED
#define LW_MATCH_INFO_INCLUDED

#include "list.h"
#include "table.h"
#include "match-info-read-iter.h"

G_BEGIN_DECLS

#define LW_TYPE_MATCH_INFO lw_match_info_get_type ()
G_DECLARE_FINAL_TYPE (LwMatchInfo, lw_match_info, LW, MATCH_INFO, LwList)

LwMatchInfo * lw_match_info_new (LwTable * haystack);


void lw_match_info_add (LwMatchInfo * self, gint column, GMatchInfo * match_info);
gboolean lw_match_info_read_column (LwMatchInfo * self, gint column, LwMatchInfoReadIter * iter);
LwTable * lw_match_info_get_haystack (LwMatchInfo * self);


G_END_DECLS

#endif

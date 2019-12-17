#ifndef LW_MATCH_INFO_INCLUDED
#define LW_MATCH_INFO_INCLUDED

#include "column-match-info.h"
#include "match-marker.h"

G_BEGIN_DECLS


/**
 * LwMatchInfo:
 * 
 * An opaque object holding the fields for #LwMatchInfo
 */
struct _LwMatchInfo {
  /*< private >*/
  GTree * tree;
  gint refs;
};
typedef struct _LwMatchInfo LwMatchInfo;


/**
 * LW_MATCH_INFO:
 * @obj: Object to cast to a #LwMatchInfo
 *
 * Returns: The object cast to a #LwMatchInfo
 */
#define LW_MATCH_INFO(obj) ((LwMatchInfo*)(obj))


LwMatchInfo * lw_match_info_new (void);
void lw_match_info_unref (LwMatchInfo * self);
LwMatchInfo * lw_match_info_ref (LwMatchInfo * self);

void lw_match_info_set_column (LwMatchInfo * self, LwQueryNodeColumnMatchInfo * column_match_info);
LwQueryNodeColumnMatchInfo * lw_match_info_get_column (LwMatchInfo * self, gint column);

G_END_DECLS

#endif

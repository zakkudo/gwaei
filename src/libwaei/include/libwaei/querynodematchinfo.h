#ifndef LW_QUERYNODEMATCHINFO_INCLUDED
#define LW_QUERYNODEMATCHINFO_INCLUDED

#include "querynodecolumnmatchinfo.h"
#include "querynodematchmarker.h"

G_BEGIN_DECLS


/**
 * LwQueryNodeMatchInfo:
 * 
 * An opaque object holding the fields for #LwQueryNodeMatchInfo
 */
struct _LwQueryNodeMatchInfo {
  /*< private >*/
  GTree * tree;
  gint refs;
};
typedef struct _LwQueryNodeMatchInfo LwQueryNodeMatchInfo;


/**
 * LW_QUERYNODEMATCHINFO:
 * @obj: Object to cast to a #LwQueryNodeMatchInfo
 *
 * Returns: The object cast to a #LwQueryNodeMatchInfo
 */
#define LW_QUERYNODEMATCHINFO(obj) ((LwQueryNodeMatchInfo*)(obj))


LwQueryNodeMatchInfo * lw_querynodematchinfo_new (void);
void lw_querynodematchinfo_unref (LwQueryNodeMatchInfo * self);
LwQueryNodeMatchInfo * lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self);

void lw_querynodematchinfo_set_column (LwQueryNodeMatchInfo * self, gint column, LwQueryNodeColumnMatchInfo * column_match_info);
LwQueryNodeColumnMatchInfo * lw_querynodematchinfo_get_column (LwQueryNodeMatchInfo * self, gint column);

G_END_DECLS

#endif

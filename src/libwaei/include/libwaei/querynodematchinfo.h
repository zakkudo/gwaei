#ifndef LW_QUERYNODEMATCHINFO_INCLUDED
#define LW_QUERYNODEMATCHINFO_INCLUDED

#include <libwaei/querynodematchmarker.h>


G_BEGIN_DECLS


struct _LwQueryNodeMatchInfoIter {
  /*< private >*/
  GList * marker;
  gint match_level;
  gchar const * END;
};
typedef struct _LwQueryNodeMatchInfoIter LwQueryNodeMatchInfoIter;


struct _LwQueryNodeMatchInfo {
  /*< private >*/
  GTree * tree;
  GList * markers;
  GList * iter;
  gboolean compiled;
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


/**
 * LwQueryNodeMatchInfo:
 * 
 * An opaque object holding the fields for #LwQueryNodeMatchInfo
 */

void lw_querynodematchinfo_unref (LwQueryNodeMatchInfo * self);
LwQueryNodeMatchInfo * lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self);

void lw_querynodematchinfo_insert (LwQueryNodeMatchInfo * self, gchar const * token, GMatchInfo * match_info);
void lw_querynodematchinfo_compile (LwQueryNodeMatchInfo * self);

G_END_DECLS

#endif

#ifndef LW_QUERYNODEMATCHINFO_INCLUDED
#define LW_QUERYNODEMATCHINFO_INCLUDED

G_BEGIN_DECLS

typedef struct _LwQueryNodeMatchInfo LwQueryNodeMatchInfo;


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


/**
 * LW_QUERYNODEMATCHINFO:
 * @obj: Object to cast to a #LwQueryNodeMatchInfo
 *
 * Returns: The object cast to a #LwQueryNodeMatchInfo
 */
#define LW_QUERYNODEMATCHINFO(ob) ((LwQueryNodeMatchInfo*)(obj))

LwQueryNodeMatchInfo * lw_querynodematchinfo_new (void);
void lw_querynodematchinfo_unref (LwQueryNodeMatchInfo * self);
LwQueryNodeMatchInfo * lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self);

G_END_DECLS

#endif

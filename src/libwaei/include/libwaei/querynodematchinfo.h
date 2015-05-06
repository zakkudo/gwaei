#ifndef LW_QUERYNODEMATCHINFO_INCLUDED
#define LW_QUERYNODEMATCHINFO_INCLUDED

G_BEGIN_DECLS

typedef struct _LwQueryNodeMatchInfo LwQueryNodeMatchInfo;

LwQueryNodeMatchInfo * lw_querynodematchinfo_new (void);
void lw_querynodematchinfo_unref (LwQueryNodeMatchInfo * self);
LwQueryNodeMatchInfo * lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self);

G_END_DECLS

#endif

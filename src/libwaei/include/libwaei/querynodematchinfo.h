#ifndef LW_QUERYNODEMATCHINFO_INCLUDED
#define LW_QUERYNODEMATCHINFO_INCLUDED

G_BEGIN_DECLS


struct _LwQueryNodeMatchInfo {
  GTree * tree;
  gint refs;
};
typedef struct _LwQueryNodeMatchInfo LwQueryNodeMatchInfo;


void lw_querynodematchinfo_add (LwQueryNodeMatchInfo * self, gint column, gint strv_index, GMatchInfo *match_info);


LwQueryNodeMatchInfo * lw_querynodematchinfo_new (void);
void lw_querynodematchinfo_unref (LwQueryNodeMatchInfo * self);
LwQueryNodeMatchInfo * lw_querynodematchinfo_ref (LwQueryNodeMatchInfo * self);



G_END_DECLS

#endif

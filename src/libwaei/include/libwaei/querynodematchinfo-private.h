#ifndef LW_QUERYNODEMATCHINFO_PRIVATE_INCLUDED
#define LW_QUERYNODEMATCHINFO_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _LwQueryNodeMatchInfo {
  GTree * tree;
  gint refs;
};

void lw_querynodematchinfo_add (LwQueryNodeMatchInfo * self, gint column, gint strv_index, GMatchInfo *match_info);

G_END_DECLS

#endif

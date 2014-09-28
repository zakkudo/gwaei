#ifndef LW_INDEXED_INCLUDED
#define LW_INDEXED_INCLUDED 

G_BEGIN_DECLS

struct _LwIndexed {
  LwParsed *parsed;
  gint ref_count;
};

typedef struct _LwIndexed LwIndexed;

#define LW_INDEXED(obj) (LwIndexed*)obj

LwIndexed* lw_indexed_new (LwParsed *parsed);

GType lw_indexed_get_type (void);
void lw_indexed_unref (LwIndexed *self);
LwIndexed* lw_indexed_ref (LwIndexed *self);



G_END_DECLS

#endif



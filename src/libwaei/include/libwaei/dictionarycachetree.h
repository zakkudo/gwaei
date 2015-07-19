#ifndef LW_DICTIONARYCACHETREE_INCLUDED
#define LW_DICTIONARYCACHETREE_INCLUDED

#include "dictionarycache.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwDictionaryCacheTree LwDictionaryCacheTree;

struct _LwDictionaryCacheTree {
  GTree * data;
  GTree * cleanup_data;
  GMutex mutex;
  gint refs;
};

//Methods
LwDictionaryCacheTree * lw_dictionarycachetree_new ();

//Properties
GType lw_dictionarycachetree_get_type ();
void lw_dictionarycachetree_insert (LwDictionaryCacheTree * self, LwDictionaryCache * cache);
void lw_dictionarycachetree_remove (LwDictionaryCacheTree * self, LwDictionaryCache * cache);
void lw_dictionarycachetree_remove_by_utf8flags (LwDictionaryCacheTree * self, LwUtf8Flag flags);
LwDictionaryCache * lw_dictionarycachetree_lookup_by_utf8flags (LwDictionaryCacheTree * self, LwUtf8Flag flags);

LwDictionaryCacheTree * lw_dictionarycachetree_ref (LwDictionaryCacheTree * self);
void lw_dictionarycachetree_unref (LwDictionaryCacheTree * self);

void lw_dictionarycachetree_lock (LwDictionaryCacheTree * self);
void lw_dictionarycachetree_unlock (LwDictionaryCacheTree * self);

G_END_DECLS

#endif


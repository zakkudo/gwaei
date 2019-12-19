#ifndef LW_DICTIONARY_CACHE_TREE_INCLUDED
#define LW_DICTIONARY_CACHE_TREE_INCLUDED

#include "dictionary-cache.h"

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
LwDictionaryCacheTree * lw_dictionary_cache_tree_new ();

//Properties
GType lw_dictionary_cache_tree_get_type ();
void lw_dictionary_cache_tree_insert (LwDictionaryCacheTree * self, LwDictionaryCache * cache);
void lw_dictionary_cache_tree_remove (LwDictionaryCacheTree * self, LwDictionaryCache * cache);
void lw_dictionary_cache_tree_remove_by_utf8flags (LwDictionaryCacheTree * self, LwUtf8Flag flags);
LwDictionaryCache * lw_dictionary_cache_tree_lookup_by_utf8flags (LwDictionaryCacheTree * self, LwUtf8Flag flags);

LwDictionaryCacheTree * lw_dictionary_cache_tree_ref (LwDictionaryCacheTree * self);
void lw_dictionary_cache_tree_unref (LwDictionaryCacheTree * self);

void lw_dictionary_cache_tree_lock (LwDictionaryCacheTree * self);
void lw_dictionary_cache_tree_unlock (LwDictionaryCacheTree * self);

G_END_DECLS

#endif


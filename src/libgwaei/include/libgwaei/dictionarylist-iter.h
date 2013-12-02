#ifndef LGW_DICTIONARYLIST_ITER_INCLUDED
#define LGW_DICTIONARYLIST_ITER_INCLUDED

G_BEGIN_DECLS


void lgw_dictionarylist_initialize_tree_iter (LgwDictionaryList *dictionary_list, GtkTreeIter *iter, gint index_);
void lgw_dictionarylist_invalidate_tree_iter (GtkTreeIter *iter);
gboolean lgw_dictionarylist_tree_iter_is_valid (LgwDictionaryList *dictionary_list, GtkTreeIter *iter);
void lgw_dictionarylist_invalidate_old_timestamps ();


G_END_DECLS

#endif

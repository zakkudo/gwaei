#ifndef LGW_DICTIONARYLIST_ITER_INCLUDED
#define LGW_DICTIONARYLIST_ITER_INCLUDED

G_BEGIN_DECLS

void lgw_dictionaryliststore_implement_treemodel_interface (GtkTreeModelIface *iface);

void lgw_dictionaryliststore_initialize_tree_iter (LgwDictionaryListStore *self, GtkTreeIter *iter, gint index_);
void lgw_dictionaryliststore_invalidate_tree_iter (GtkTreeIter *iter);
gboolean lgw_dictionaryliststore_tree_iter_is_valid (LgwDictionaryListStore *self, GtkTreeIter *iter);
void lgw_dictionaryliststore_invalidate_old_timestamps ();

G_END_DECLS

#endif

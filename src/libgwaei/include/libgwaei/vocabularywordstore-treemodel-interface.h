#ifndef LGW_VOCABULARYWORDSTORE_TREEMODEL_INTERFACE_INCLUDED
#define LGW_VOCABULARYWORDSTORE_TREEMODEL_INTERFACE_INCLUDED

G_BEGIN_DECLS

void lgw_vocabularywordstore_implement_treemodel_interface (GtkTreeModelIface *iface);

void lgw_vocabularywordstore_initialize_tree_iter (LgwVocabularyWordStore *self, GtkTreeIter *iter, gint index_);
void lgw_vocabularywordstore_invalidate_tree_iter (GtkTreeIter *iter);
gboolean lgw_vocabularywordstore_tree_iter_is_valid (LgwVocabularyWordStore *self, GtkTreeIter *iter);
void lgw_vocabularywordstore_invalidate_old_timestamps ();

G_END_DECLS

#endif


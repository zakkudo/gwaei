#ifndef LGW_VOCABULARYLISTSTORE_TREEMODEL_INTERFACE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_TREEMODEL_INTERFACE_INCLUDED

G_BEGIN_DECLS

void lgw_vocabularyliststore_implement_treemodel_interface (GtkTreeModelIface *iface);

void lgw_vocabularyliststore_initialize_tree_iter (LgwVocabularyListStore *self, GtkTreeIter *iter, gint index_);
void lgw_vocabularyliststore_invalidate_tree_iter (GtkTreeIter *iter);
gboolean lgw_vocabularyliststore_tree_iter_is_valid (LgwVocabularyListStore *self, GtkTreeIter *iter);
void lgw_vocabularyliststore_invalidate_old_timestamps ();

G_END_DECLS

#endif

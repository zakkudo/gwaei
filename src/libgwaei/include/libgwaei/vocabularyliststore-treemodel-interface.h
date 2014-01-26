#ifndef LGW_VOCABULARYLISTSTORE_TREEMODEL_INTERFACE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_TREEMODEL_INTERFACE_INCLUDED

G_BEGIN_DECLS

void lgw_vocabularyliststore_init_interface (GtkTreeModelIface *iface);


void lgw_vocabularyliststore_initialize_tree_iter (LgwVocabularyListStore *vocabulary_list_store, GtkTreeIter *iter, gint index_);
void lgw_vocabularyliststore_invalidate_tree_iter (GtkTreeIter *iter);
gboolean lgw_vocabularyliststore_tree_iter_is_valid (LgwVocabularyListStore *vocabulary_list_store, GtkTreeIter *iter);
void lgw_vocabularyliststore_invalidate_old_timestamps ();


GtkTreeModelFlags lgw_vocabularyliststore_get_flags (GtkTreeModel *tree_model);
gint lgw_vocabularyliststore_get_n_columns (GtkTreeModel *tree_model);
GType lgw_vocabularyliststore_get_column_type (GtkTreeModel *tree_model, gint index_);
gboolean lgw_vocabularyliststore_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
GtkTreePath* lgw_vocabularyliststore_get_path (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_vocabularyliststore_get_value (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
gboolean lgw_vocabularyliststore_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_vocabularyliststore_iter_previous (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_vocabularyliststore_iter_children (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
gboolean lgw_vocabularyliststore_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter);
gint lgw_vocabularyliststore_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_vocabularyliststore_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
gboolean lgw_vocabularyliststore_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);
void lgw_vocabularyliststore_ref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_vocabularyliststore_unref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);

G_END_DECLS

#endif

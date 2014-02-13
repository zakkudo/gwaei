#ifndef LGW_VOCABULARYWORDSTORE_TREEMODEL_INTERFACE_INCLUDED
#define LGW_VOCABULARYWORDSTORE_TREEMODEL_INTERFACE_INCLUDED

G_BEGIN_DECLS

void lgw_vocabularywordstore_init_interface (GtkTreeModelIface *iface);

void lgw_vocabularywordstore_initialize_tree_iter (LgwVocabularyWordStore *self, GtkTreeIter *iter, gint index_);
void lgw_vocabularywordstore_invalidate_tree_iter (GtkTreeIter *iter);
gboolean lgw_vocabularywordstore_tree_iter_is_valid (LgwVocabularyWordStore *self, GtkTreeIter *iter);
void lgw_vocabularywordstore_invalidate_old_timestamps ();


GtkTreeModelFlags lgw_vocabularywordstore_get_flags (GtkTreeModel *tree_model);
gint lgw_vocabularywordstore_get_n_columns (GtkTreeModel *tree_model);
GType lgw_vocabularywordstore_get_column_type (GtkTreeModel *tree_model, gint index_);
gboolean lgw_vocabularywordstore_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
GtkTreePath* lgw_vocabularywordstore_get_path (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_vocabularywordstore_get_value (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
gboolean lgw_vocabularywordstore_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_vocabularywordstore_iter_previous (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_vocabularywordstore_iter_children (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
gboolean lgw_vocabularywordstore_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter);
gint lgw_vocabularywordstore_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_vocabularywordstore_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
gboolean lgw_vocabularywordstore_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);
void lgw_vocabularywordstore_ref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_vocabularywordstore_unref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);

G_END_DECLS

#endif


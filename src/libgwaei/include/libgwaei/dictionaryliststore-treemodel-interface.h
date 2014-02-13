#ifndef LGW_DICTIONARYLIST_ITER_INCLUDED
#define LGW_DICTIONARYLIST_ITER_INCLUDED

G_BEGIN_DECLS

void lgw_dictionaryliststore_init_interface (GtkTreeModelIface *iface);

void lgw_dictionaryliststore_initialize_tree_iter (LgwDictionaryListStore *self, GtkTreeIter *iter, gint index_);
void lgw_dictionaryliststore_invalidate_tree_iter (GtkTreeIter *iter);
gboolean lgw_dictionaryliststore_tree_iter_is_valid (LgwDictionaryListStore *self, GtkTreeIter *iter);
void lgw_dictionaryliststore_invalidate_old_timestamps ();


GtkTreeModelFlags lgw_dictionaryliststore_get_flags (GtkTreeModel *tree_model);
gint lgw_dictionaryliststore_get_n_columns (GtkTreeModel *tree_model);
GType lgw_dictionaryliststore_get_column_type (GtkTreeModel *tree_model, gint index_);
gboolean lgw_dictionaryliststore_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
GtkTreePath* lgw_dictionaryliststore_get_path (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_dictionaryliststore_get_value (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
gboolean lgw_dictionaryliststore_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_dictionaryliststore_iter_previous (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_dictionaryliststore_iter_children (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
gboolean lgw_dictionaryliststore_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter);
gint lgw_dictionaryliststore_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_dictionaryliststore_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
gboolean lgw_dictionaryliststore_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);
void lgw_dictionaryliststore_ref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_dictionaryliststore_unref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);


G_END_DECLS

#endif

#ifndef LGW_DICTIONARYLIST_CALLBACKS_INCLUDED
#define LGW_DICTIONARYLIST_CALLBACKS_INCLUDED


void lgw_dictionarylist_connect_signals (LgwDictionaryList *dictionary_list);
void lgw_dictionarylist_disconnect_signals (LgwDictionaryList *dictionarylist);


GtkTreeModelFlags lgw_dictionarylist_get_flags (GtkTreeModel *tree_model);
gint lgw_dictionarylist_get_n_columns (GtkTreeModel *tree_model);
GType lgw_dictionarylist_get_column_type (GtkTreeModel *tree_model, gint index_);
gboolean lgw_dictionarylist_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
GtkTreePath* lgw_dictionarylist_get_path (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_dictionarylist_get_value (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
gboolean lgw_dictionarylist_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_dictionarylist_iter_previous (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_dictionarylist_iter_children (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
gboolean lgw_dictionarylist_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter);
gint lgw_dictionarylist_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter);
gboolean lgw_dictionarylist_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
gboolean lgw_dictionarylist_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);
void lgw_dictionarylist_ref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);
void lgw_dictionarylist_unref_node (GtkTreeModel *tree_model, GtkTreeIter *iter);

#endif


#ifndef LGW_VOCABULARYLISTSTORE_TREESORTABLE_INTERFACE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_TREESORTABLE_INTERFACE_INCLUDED

G_BEGIN_DECLS

void lgw_vocabularyliststore_implement_treesortable_interface (GtkTreeSortableIface *iface);

gint lgw_vocabularyliststore_name_compare_func (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);
gint lgw_vocabularyliststore_saved_position_compare_func (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);

G_END_DECLS

#endif

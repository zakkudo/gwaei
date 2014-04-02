#ifndef LGW_VOCABULARYWORDSTORE_TREESORTABLE_INTERFACE_INCLUDED
#define LGW_VOCABULARYWORDSTORE_TREESORTABLE_INTERFACE_INCLUDED

G_BEGIN_DECLS

void lgw_vocabularywordstore_implement_treesortable_interface (GtkTreeSortableIface *iface);

gint lgw_vocabularywordstore_kanji_compare_func (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);
gint lgw_vocabularywordstore_reading_compare_func (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);
gint lgw_vocabularywordstore_definition_compare_func (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);
gint lgw_vocabularywordstore_saved_position_compare_func (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data);

G_END_DECLS

#endif


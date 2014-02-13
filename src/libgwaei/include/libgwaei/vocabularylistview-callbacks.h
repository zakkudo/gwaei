#ifndef LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED
#define LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED

G_BEGIN_DECLS


void lgw_vocabularylistview_connect_signals (LgwVocabularyListView *self);
void lgw_vocabularylistview_disconnect_signals (LgwVocabularyListView *self);

void lgw_vocabularylistview_selection_changed_cb (LgwVocabularyListView *self, GtkTreeSelection *tree_selection);

void lgw_vocabularylistview_add_new_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularylistview_delete_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

gboolean lgw_vocabularylistview_focus_in_event_cb (LgwVocabularyListView *self, GdkEvent *event, GtkTreeView *inner_tree_view);
gboolean lgw_vocabularylistview_focus_out_event_cb (LgwVocabularyListView *self, GdkEvent *event, GtkTreeView *inner_tree_view);


G_END_DECLS

#endif



#ifndef LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED
#define LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED

G_BEGIN_DECLS


void lgw_vocabularylistview_connect_signals (LgwVocabularyListView *vocabulary_list_view);
void lgw_vocabularylistview_disconnect_signals (LgwVocabularyListView *vocabulary_list_view);

void lgw_vocabularylistview_selection_changed_cb (LgwVocabularyListView *vocabulary_list_view, GtkTreeSelection *tree_selection);

void lgw_vocabularylistview_add_new_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularylistview_remove_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

gboolean lgw_vocabularylistview_focus_in_event_cb (LgwVocabularyListView *vocabulary_list_view, GdkEvent *event, GtkTreeView *inner_tree_view);
gboolean lgw_vocabularylistview_focus_out_event_cb (LgwVocabularyListView *vocabulary_list_view, GdkEvent *event, GtkTreeView *inner_tree_view);


G_END_DECLS

#endif



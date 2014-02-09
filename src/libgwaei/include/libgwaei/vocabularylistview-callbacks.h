#ifndef LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED
#define LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED

G_BEGIN_DECLS


void lgw_vocabularylistview_connect_signals (LgwVocabularyListView *vocabulary_list_view);
void lgw_vocabularylistview_disconnect_signals (LgwVocabularyListView *vocabulary_list_view);

void lgw_vocabularylistview_selection_changed_cb (LgwVocabularyListView *vocabulary_list_view, GtkTreeSelection *tree_selection);

void lgw_vocabularylistview_new_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularylistview_remove_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);


G_END_DECLS

#endif


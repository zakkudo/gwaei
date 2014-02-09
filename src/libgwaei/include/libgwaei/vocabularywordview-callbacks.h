#ifndef LGW_VOCABULARYWORDVIEW_CALLBACKS_INCLUDED
#define LGW_VOCABULARYWORDVIEW_CALLBACKS_INCLUDED

G_BEGIN_DECLS


void lgw_vocabularywordview_connect_signals (LgwVocabularyWordView *vocabulary_word_view);
void lgw_vocabularywordview_disconnect_signals (LgwVocabularyWordView *vocabulary_word_view);

void lgw_vocabularywordview_selection_changed_cb (LgwVocabularyWordView *vocabulary_word_view, GtkTreeSelection *tree_selection);

void lgw_vocabularywordview_add_new_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularywordview_remove_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);


G_END_DECLS

#endif



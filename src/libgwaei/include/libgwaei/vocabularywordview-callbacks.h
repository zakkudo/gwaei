#ifndef LGW_VOCABULARYWORDVIEW_CALLBACKS_INCLUDED
#define LGW_VOCABULARYWORDVIEW_CALLBACKS_INCLUDED

G_BEGIN_DECLS


void lgw_vocabularywordview_connect_signals (LgwVocabularyWordView *self);
void lgw_vocabularywordview_disconnect_signals (LgwVocabularyWordView *self);

void lgw_vocabularywordview_save_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularywordview_add_new_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularywordview_delete_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularywordview_revert_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularywordview_cut_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularywordview_copy_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularywordview_paste_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);


G_END_DECLS

#endif



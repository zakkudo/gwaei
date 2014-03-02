#ifndef LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED
#define LGW_VOCABULARYLISTVIEW_CALLBACKS_INCLUDED

G_BEGIN_DECLS


void lgw_vocabularylistview_connect_signals (LgwVocabularyListView *self);
void lgw_vocabularylistview_disconnect_signals (LgwVocabularyListView *self);

void lgw_vocabularylistview_add_new_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_vocabularylistview_delete_selected_activated_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

G_END_DECLS

#endif



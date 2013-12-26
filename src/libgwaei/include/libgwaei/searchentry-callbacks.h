#ifndef LGW_SEARCHENTRY_CALLBACKS_INCLUDED
#define LGW_SEARCHENTRY_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_searchentry_connect_signals (LgwSearchEntry *entry);
void lgw_searchentry_disconnect_signals (LgwSearchEntry *entry);

void lgw_searchentry_changed_cb (GtkSearchEntry *search_entry, gpointer data);
void lgw_searchentry_activated_cb (GtkEntry *entry, gpointer data);

void lgw_searchentry_insert_unknown_character_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_word_edge_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_not_word_edge_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_and_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_or_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_clear_search_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

G_END_DECLS

#endif

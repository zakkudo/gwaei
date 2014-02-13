#ifndef LGW_SEARCHENTRY_CALLBACKS_INCLUDED
#define LGW_SEARCHENTRY_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_searchentry_connect_signals (LgwSearchEntry *entry);
void lgw_searchentry_disconnect_signals (LgwSearchEntry *entry);

void lgw_searchentry_changed_cb (LgwSearchEntry *self, GtkSearchEntry *inner_search_entry);
void lgw_searchentry_activated_cb (LgwSearchEntry *self, GtkEntry *inner_entry);

gboolean lgw_searchentry_focus_in_event_cb (LgwSearchEntry *self, GdkEvent *event, GtkSearchEntry *inner_search_entry);
gboolean lgw_searchentry_focus_out_event_cb (LgwSearchEntry *self, GdkEvent *event, GtkSearchEntry *inner_search_entry);

void lgw_searchentry_insert_unknown_character_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_word_edge_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_not_word_edge_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_and_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_insert_or_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_clear_search_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

void lgw_searchentry_copy_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_cut_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void lgw_searchentry_paste_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

G_END_DECLS

#endif

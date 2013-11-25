#ifndef LGW_SEARCHENTRY_CALLBACKS_INCLUDED
#define LGW_SEARCHENTRY_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_searchentry_connect_signals (LgwSearchEntry *entry);
void lgw_searchentry_disconnect_signals (LgwSearchEntry *entry);

void lgw_searchentry_changed_cb (GtkEditable *editable, gpointer data);
void lgw_searchentry_activated_cb (GtkEntry *entry, gpointer data);


G_END_DECLS

#endif

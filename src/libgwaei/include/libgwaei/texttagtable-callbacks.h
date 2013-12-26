#ifndef LGW_TEXTTAGTABLE_CALLBACKS_INCLUDED
#define LGW_TEXTTAGTABLE_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_texttagtable_connect_signals (LgwTextTagTable *tag_table);
void lgw_texttagtable_disconnect_signals (LgwTextTagTable *tag_table);

void lgw_texttagtable_sync_tag_cb (GSettings *settings, gchar *key, gpointer data);

G_END_DECLS

#endif

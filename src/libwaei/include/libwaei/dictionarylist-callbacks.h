#ifndef LW_DICTIONARYLIST_CALLBACKS_INCLUDED
#define LW_DICTIONARYLIST_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lw_dictionarylist_connect_signals (LwDictionaryList *dictionarylist);
void lw_dictionarylist_disconnect_signals (LwDictionaryList *dictionarylist);

void lw_dictionarylist_inserted_cb (LwDictionaryList *dictionary_list, gint position, gpointer data);
void lw_dictionarylist_deleted_cb (LwDictionaryList *dictionary_list, gint position, gpointer data);
void lw_dictionarylist_reordered_cb (LwDictionaryList *dictionary_list, gint *new_order, gpointer data);


G_END_DECLS

#endif


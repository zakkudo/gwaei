#ifndef LW_DICTIONARYCALLBACKS_INCLUDED
#define LW_DICTIONARYCALLBACKS_INCLUDED

G_BEGIN_DECLS

void lw_dictionary_sync_downloadlist_cb (GSettings*, gchar*, gpointer);
void lw_dictionary_sync_progress_cb (LwDictionary *dictionary, LwProgress *progress);

G_END_DECLS

#endif


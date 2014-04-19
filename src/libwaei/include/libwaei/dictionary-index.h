#ifndef LW_DICTIONARY_INDEX_INCLUDED
#define LW_DICTIONARY_INDEX_INCLUDED

G_BEGIN_DECLS


GHashTable* lw_dictionary_index_search (LwDictionary *dictionary, LwMorphologyString *morphologystring, LwIndexFlag flags, LwProgress *progress);
void lw_dictionary_index_create (LwDictionary *dictionary, LwProgress*progress);
gboolean lw_dictionary_index_load (LwDictionary *dictionary, LwProgress*progress);

gboolean lw_dictionary_index_is_valid (LwDictionary *dictionary);
gboolean lw_dictionary_index_is_loaded (LwDictionary *dictionary);
gboolean lw_dictionary_index_exists (LwDictionary *dictionary);
gchar* lw_dictionary_index_get_path (LwDictionary *dictionary);


G_END_DECLS

#endif

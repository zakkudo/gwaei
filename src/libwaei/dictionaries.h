#ifndef LW_DICTIONARIES_INCLUDED
#define LW_DICTIONARIES_INCLUDED

#include "list.h"
#include "dictionary.h"

G_BEGIN_DECLS

//Preference Schema
#define LW_SCHEMA_DICTIONARY       "org.gnome.gwaei.dictionary"

//Boilerplate
#define LW_TYPE_DICTIONARIES lw_dictionary_get_type ()
G_DECLARE_FINAL_TYPE (LwDictionaries, lw_dictionaries, LW, DICTIONARIES, LwList)

//Methods
LwDictionaries * lw_dictionaries_new (LwPreferences * preferences);

G_END_DECLS


#endif

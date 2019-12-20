#ifndef LW_DICTIONARY_INSTALLS_INCLUDED
#define LW_DICTIONARY_INSTALLS_INCLUDED

#include "dictionary-install.h"

G_BEGIN_DECLS

//Preference Schema
#define LW_SCHEMA_DICTIONARY       "org.gnome.gwaei.dictionary"

//Preference Settings
#define LW_KEY_ENGLISH_SOURCE      "english-source"
#define LW_KEY_KANJI_SOURCE        "kanji-source"
#define LW_KEY_NAMES_PLACES_SOURCE "names-places-source"
#define LW_KEY_EXAMPLES_SOURCE     "examples-source"
#define LW_KEY_ORDER               "order"

//Boilerplate
#define LW_TYPE_DICTIONARY_INSTALLS lw_dictionary_installs_get_type ()
G_DECLARE_FINAL_TYPE (LwDictionaryInstalls, lw_dictionary_installs, LW, DICTIONARY_INSTALLS, LwList)

//Methods
LwDictionaryInstalls* lw_dictionary_installs_new (LwPreferences *preferences);
GType lw_dictionary_installs_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif

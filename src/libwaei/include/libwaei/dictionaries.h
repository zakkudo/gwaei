#ifndef LW_DICTIONARIES_INCLUDED
#define LW_DICTIONARIES_INCLUDED

#include "iterable.h"
#include "dictionary.h"

G_BEGIN_DECLS

//Preference Schema
#define LW_SCHEMA_DICTIONARY       "org.gnome.gwaei.dictionary"

//Boilerplate
#define LW_TYPE_DICTIONARIES lw_dictionary_get_type ()
G_DECLARE_FINAL_TYPE (LwDictionaries, lw_dictionaries, LW, DICTIONARIES, LwIterable)

struct _LwDictionaries {
  GObject object;
};

//Methods
LwDictionaries* lw_dictionaries_new (LwPreferences *preferences);

void lw_dictionaries_load_order (LwDictionaries *self);

GList* lw_dictionaries_dictionaries (LwDictionaries *self);

LwDictionary* lw_dictionaries_find (LwDictionaries* self, const GType TYPE, const gchar* FILENAME);
LwDictionary* lw_dictionaries_fuzzy_find (LwDictionaries *self, const gchar *FUZZY_DESCRIPTION);
LwDictionary* lw_dictionaries_find_by_filename (LwDictionaries *self, const gchar *FILENAME);
LwDictionary* lw_dictionaries_find_by_id (LwDictionaries *self, const gchar *ID);

void lw_dictionaries_load_installed (LwDictionaries *self);

GMenuModel* lw_dictionaries_get_menumodel (LwDictionaries *self);

gboolean lw_dictionaries_dictionary_exists (LwDictionaries *self, LwDictionary *dictionary);

LwDictionary* lw_dictionaries_nth (LwDictionaries *self, gint index);
GList* lw_dictionaries_remove (LwDictionaries *self, gint *indices);
void lw_dictionaries_clear (LwDictionaries *self);
gint lw_dictionaries_length (LwDictionaries *self);
gint* lw_dictionaries_get_indices (LwDictionaries *self, GList *dictionaries);

G_END_DECLS


#endif

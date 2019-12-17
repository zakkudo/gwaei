#ifndef LW_DICTIONARY_INSTALLS_INCLUDED
#define LW_DICTIONARY_INSTALLS_INCLUDED

#include <libwaei/dictionary.h>

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
typedef struct _LwDictionaryInstalls LwDictionaryInstalls;
typedef struct _LwDictionaryInstallsClass LwDictionaryInstallsClass;
typedef struct _LwDictionaryInstallsPrivate LwDictionaryInstallsPrivate;
typedef struct _LwDictionaryInstallsClassPrivate LwDictionaryInstallsClassPrivate;

#define LW_TYPE_DICTIONARY_INSTALLS              (lw_dictionary_installs_get_type())
#define LW_DICTIONARY_INSTALLS(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARY_INSTALLS, LwDictionaryInstalls))
#define LW_DICTIONARY_INSTALLS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARY_INSTALLS, LwDictionaryInstallsClass))
#define LW_IS_DICTIONARY_INSTALLS(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARY_INSTALLS))
#define LW_IS_DICTIONARY_INSTALLS_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARY_INSTALLS))
#define LW_DICTIONARY_INSTALLS_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARY_INSTALLS, LwDictionaryInstallsClass))


struct _LwDictionaryInstalls {
  GObject object;
};

struct _LwDictionaryInstallsClass {
  GObjectClass parent_class;
  LwDictionaryInstallsClassPrivate *priv;

  //Signals
  void (*row_changed) (LwDictionaryInstalls* dictionary_installs, gint position, gpointer data);
  void (*row_inserted) (LwDictionaryInstalls* dictionary_installs, gint position, gpointer data);
  void (*row_deleted) (LwDictionaryInstalls* dictionary_installs, gint position, gpointer data);
  void (*rows_reordered) (LwDictionaryInstalls* dictionary_installs, gint *order, gpointer data);
};


//Methods
LwDictionaryInstalls* lw_dictionary_installs_new (LwPreferences *preferences);
GType lw_dictionary_installs_get_type (void) G_GNUC_CONST;


gint lw_dictionary_installs_length (LwDictionaryInstalls *self);
LwDictionaryInstall* lw_dictionary_installs_nth (LwDictionaryInstalls *self, gint position);

gint* lw_dictionary_installs_get_indices (LwDictionaryInstalls *self, GList *dictionaryinstalls);


gint* lw_dictionary_installs_insert (LwDictionaryInstalls *self, gint position, GList *dictionary_installs);
GList* lw_dictionary_installs_remove (LwDictionaryInstalls *self, gint *indices);
void lw_dictionary_installs_clear (LwDictionaryInstalls *self);

GList* lw_dictionary_installs_dictionaryinstalls (LwDictionaryInstalls *self);

void lw_dictionary_installs_load_default (LwDictionaryInstalls *self);

LwDictionaryInstall* lw_dictionary_installs_fuzzy_find (LwDictionaryInstalls *self, const gchar *DESCRIPTION);

GList* lw_dictionary_installs_build_transaction (LwDictionaryInstalls *self, GList *dictionaryinstalls);

G_END_DECLS


#endif

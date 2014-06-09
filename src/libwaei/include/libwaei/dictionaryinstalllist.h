#ifndef LW_DICTIONARYINSTALLLIST_INCLUDED
#define LW_DICTIONARYINSTALLLIST_INCLUDED

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
typedef struct _LwDictionaryInstallList LwDictionaryInstallList;
typedef struct _LwDictionaryInstallListClass LwDictionaryInstallListClass;
typedef struct _LwDictionaryInstallListPrivate LwDictionaryInstallListPrivate;
typedef struct _LwDictionaryInstallListClassPrivate LwDictionaryInstallListClassPrivate;

#define LW_TYPE_DICTIONARYINSTALLLIST              (lw_dictionaryinstalllist_get_type())
#define LW_DICTIONARYINSTALLLIST(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARYINSTALLLIST, LwDictionaryInstallList))
#define LW_DICTIONARYINSTALLLIST_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARYINSTALLLIST, LwDictionaryInstallListClass))
#define LW_IS_DICTIONARYINSTALLLIST(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARYINSTALLLIST))
#define LW_IS_DICTIONARYINSTALLLIST_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARYINSTALLLIST))
#define LW_DICTIONARYINSTALLLIST_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARYINSTALLLIST, LwDictionaryInstallListClass))


struct _LwDictionaryInstallList {
  GObject object;
  LwDictionaryInstallListPrivate *priv;
};

struct _LwDictionaryInstallListClass {
  GObjectClass parent_class;
  LwDictionaryInstallListClassPrivate *priv;

  //Signals
  void (*row_changed) (LwDictionaryInstallList* dictionaryinstalllist, gint position, gpointer data);
  void (*row_inserted) (LwDictionaryInstallList* dictionaryinstalllist, gint position, gpointer data);
  void (*row_deleted) (LwDictionaryInstallList* dictionaryinstalllist, gint position, gpointer data);
  void (*rows_reordered) (LwDictionaryInstallList* dictionaryinstalllist, gint *order, gpointer data);
};


//Methods
LwDictionaryInstallList* lw_dictionaryinstalllist_new (LwPreferences *preferences);
GType lw_dictionaryinstalllist_get_type (void) G_GNUC_CONST;


gint lw_dictionaryinstalllist_length (LwDictionaryInstallList *self);
LwDictionaryInstall* lw_dictionaryinstalllist_nth (LwDictionaryInstallList *self, gint position);

gint* lw_dictionaryinstalllist_get_indices (LwDictionaryInstallList *self, GList *dictionaryinstalls);


gint* lw_dictionaryinstalllist_insert (LwDictionaryInstallList *self, gint position, GList *dictionaryinstalllist);
GList* lw_dictionaryinstalllist_remove (LwDictionaryInstallList *self, gint *indices);
void lw_dictionaryinstalllist_clear (LwDictionaryInstallList *self);

GList* lw_dictionaryinstalllist_dictionaryinstalls (LwDictionaryInstallList *self);

void lw_dictionaryinstalllist_load_default (LwDictionaryInstallList *self);

G_END_DECLS


#endif

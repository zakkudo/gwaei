#ifndef LW_DICTIONARYLIST_INCLUDED
#define LW_DICTIONARYLIST_INCLUDED

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
typedef struct _LwDictionaryList LwDictionaryList;
typedef struct _LwDictionaryListClass LwDictionaryListClass;
typedef struct _LwDictionaryListPrivate LwDictionaryListPrivate;
typedef struct _LwDictionaryListClassPrivate LwDictionaryListClassPrivate;

#define LW_TYPE_DICTIONARYLIST              (lw_dictionarylist_get_type())
#define LW_DICTIONARYLIST(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARYLIST, LwDictionaryList))
#define LW_DICTIONARYLIST_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARYLIST, LwDictionaryListClass))
#define LW_IS_DICTIONARYLIST(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARYLIST))
#define LW_IS_DICTIONARYLIST_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARYLIST))
#define LW_DICTIONARYLIST_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARYLIST, LwDictionaryListClass))


struct _LwDictionaryList {
  GObject object;
  LwDictionaryListPrivate *priv;
};

struct _LwDictionaryListClass {
  GObjectClass parent_class;
  LwDictionaryListClassPrivate *priv;

  //Signals
  void (*row_changed) (LwDictionaryList* dictionarylist, gint position, gpointer data);
  void (*row_inserted) (LwDictionaryList* dictionarylist, gint position, gpointer data);
  void (*row_deleted) (LwDictionaryList* dictionarylist, gint position, gpointer data);
  void (*rows_reordered) (LwDictionaryList* dictionarylist, gint *order, gpointer data);
};


//Methods
LwDictionaryList* lw_dictionarylist_new (LwPreferences *preferences);
GType lw_dictionarylist_get_type (void) G_GNUC_CONST;


gint lw_dictionarylist_get_total (LwDictionaryList *self);

void lw_dictionarylist_load_installed (LwDictionaryList *self, LwMorphologyEngine *morphologyengine);
void lw_dictionarylist_load_installable (LwDictionaryList *self);

void  lw_dictionarylist_load_order (LwDictionaryList *self);

void lw_dictionarylist_load_order (LwDictionaryList *self);

GList* lw_dictionarylist_dictionaries (LwDictionaryList *self);

LwDictionary* lw_dictionarylist_get_dictionary_by_position (LwDictionaryList* self, gint position);
LwDictionary* lw_dictionarylist_get_dictionary_by_filename (LwDictionaryList *self, const gchar *FILENAME);
LwDictionary* lw_dictionarylist_get_dictionary_fuzzy (LwDictionaryList *self, const gchar *FUZZY_DESCRIPTION);
LwDictionary* lw_dictionarylist_get_dictionary_by_id (LwDictionaryList *self, const gchar *ENGINE_AND_FILENAME);

GMenuModel* lw_dictionarylist_get_menumodel (LwDictionaryList *self);

gboolean lw_dictionarylist_dictionary_exists (LwDictionaryList *self, LwDictionary *dictionary);

LwDictionary* lw_dictionarylist_nth (LwDictionaryList *self, gint index);
GList* lw_dictionarylist_remove (LwDictionaryList *self, gint *indices);
void lw_dictionarylist_clear (LwDictionaryList *self);
gint lw_dictionarylist_length (LwDictionaryList *self);
gint* lw_dictionarylist_get_indices (LwDictionaryList *self, GList *dictionaries);

G_END_DECLS


#endif

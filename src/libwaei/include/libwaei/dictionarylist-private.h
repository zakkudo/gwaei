#ifndef LW_DICTIONARYLIST_PRIVATE_INCLUDED
#define LW_DICTIONARYLIST_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_PREFERENCES,
    TOTAL_PROPS
} Props;


typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_INSERTED,
  SIGNALID_DELETED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

typedef enum {
  CLASS_SIGNALID_ROW_CHANGED,
  CLASS_SIGNALID_ROW_INSERTED,
  CLASS_SIGNALID_ROW_DELETED,
  CLASS_SIGNALID_ROWS_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _Index {
  GHashTable *typename;
  GHashTable *filename;
  GHashTable *id;
};

struct _Data {
  struct _Index index;
  GList *list; //<!-- A Glist of LwWord
  GList **array;
  gint length;
  gboolean changed; //<!-- hint of there are any savable changes
  gboolean loaded;
  GMenuModel *menu_model;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _Config {
  LwPreferences *preferences;
};

struct _LwDictionaryListPrivate {
  struct _Data data;
  struct _Config config;
};

struct _LwDictionaryListClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_DICTIONARYLIST_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYLIST, LwDictionaryListPrivate));

void lw_dictionarylist_menumodel_insert (LwDictionaryList *dictioanry_list, LwDictionary *dictionary, gint index_);
void lw_dictionarylist_menumodel_append (LwDictionaryList *dictionary_list, LwDictionary *dictionary);
void lw_dictionarylist_sync_menumodel (LwDictionaryList *dictionary_list);

void lw_dictionarylist_set_preferences (LwDictionaryList *dictionary_list, LwPreferences *preferences);
LwPreferences* lw_dictionarylist_get_preferences (LwDictionaryList *dictionary_list);

void lw_dictionarylist_set_menumodel (LwDictionaryList *dictionary_list, GMenuModel *menu_model);

G_END_DECLS

#include <libwaei/dictionarylist-callbacks.h>

#endif



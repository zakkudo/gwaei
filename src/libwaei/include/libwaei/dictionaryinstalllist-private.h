#ifndef LW_DICTIONARYINSTALLLIST_PRIVATE_INCLUDED
#define LW_DICTIONARYINSTALLLIST_PRIVATE_INCLUDED

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
};


struct _Config {
  LwPreferences *preferences;
};

struct _LwDictionaryInstallListPrivate {
  struct _Data data;
  struct _Config config;
};

struct _LwDictionaryInstallListClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

void lw_dictionaryinstalllist_menumodel_insert (LwDictionaryInstallList *dictioanry_list, LwDictionary *dictionary, gint index_);
void lw_dictionaryinstalllist_menumodel_append (LwDictionaryInstallList *dictionary_list, LwDictionary *dictionary);
void lw_dictionaryinstalllist_sync_menumodel (LwDictionaryInstallList *dictionary_list);

void lw_dictionaryinstalllist_set_preferences (LwDictionaryInstallList *dictionary_list, LwPreferences *preferences);
LwPreferences* lw_dictionaryinstalllist_get_preferences (LwDictionaryInstallList *dictionary_list);

void lw_dictionaryinstalllist_set_menumodel (LwDictionaryInstallList *dictionary_list, GMenuModel *menu_model);

G_END_DECLS

#include <libwaei/dictionaryinstalllist-callbacks.h>

#endif



#ifndef LGW_VOCABULARYLISTSTORE_PRIVATE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_PRIVATE_INCLUDED

G_BEGIN_DECLS


typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_DELETED,
  SIGNALID_INSERTED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

typedef enum {
  PROP_0,
  PROP_PREFERENCES,
  TOTAL_PROPS
} Prop;

struct _Index {
  GHashTable *filename;
  GHashTable *wordstore;
  GHashTable *update_filename_callback;
};

struct _Data {
  gint list_new_index;
  GList *list; //!< LgwVocabularyWordStore
  GMenuModel *menumodel;
  gboolean has_changes;
  gint length;
  struct _Index index;
  GList **array;
};


struct _Config {
  LwPreferences *preferences;
};


struct _LgwVocabularyListStorePrivate {
  struct _Config config;
  struct _Data data;
  GFileMonitor *file_monitor;
};

struct _LgwVocabularyListStoreClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_VOCABULARYLISTSTORE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStorePrivate))

void lgw_vocabularyliststore_invalidate_length (LgwVocabularyListStore *self);

G_END_DECLS

#include <libgwaei/vocabularyliststore-treemodel-interface.h>
#include <libgwaei/vocabularyliststore-treedragdest-interface.h>
#include <libgwaei/vocabularyliststore-treedragsource-interface.h>

#endif


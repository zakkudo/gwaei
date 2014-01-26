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
  CLASS_SIGNALID_CHANGED,
  CLASS_SIGNALID_INSERTED,
  CLASS_SIGNALID_DELETED,
  CLASS_SIGNALID_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

typedef enum {
  PROP_0,
  TOTAL_PROPS
} Prop;


struct _Data {
  gint list_new_index;
  GList *list; //!< LgwVocabularyWordStore
  GMenuModel *menumodel;
  gboolean has_changes;
  gint length;
};


struct _Config {
  gboolean unused;
};


struct _LgwVocabularyListStorePrivate {
  struct _Config config;
  struct _Data data;
  GFileMonitor *file_monitor;
};

struct _LgwVocabularyListStoreClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_VOCABULARYLISTSTORE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStorePrivate))

G_END_DECLS

#include <libgwaei/vocabularyliststore-treemodel-interface.h>

#endif


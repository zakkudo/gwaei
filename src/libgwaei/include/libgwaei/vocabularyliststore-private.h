#ifndef LGW_VOCABULARYLISTSTORE_PRIVATE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  CLASS_SIGNALID_CHANGED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LgwVocabularyListStorePrivate {
  GMenuModel *menumodel;
  gboolean has_changes;
  gint list_new_index;
};

struct _LgwVocabularyListStoreClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LGW_VOCABULARYLISTSTORE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStorePrivate))

G_END_DECLS

#include <libgwaei/vocabularyliststore-treemodel-interface.h>

#endif


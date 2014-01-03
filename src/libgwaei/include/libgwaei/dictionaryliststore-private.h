#ifndef LGW_DICTIONARYLISTSTORE_PRIVATE_INCLUDED
#define LGW_DICTIONARYLISTSTORE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_DELETED,
  SIGNALID_INSERTED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

struct _Data {
  LgwDictionaryListStore *dictionary_list_store;
  GMenuModel *menumodel;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _LgwDictionaryListStorePrivate {
  struct _Data data;
};

struct _LgwDictionaryListStoreClassPrivate {
  gint unused;
};

#define LGW_DICTIONARYLISTSTORE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_DICTIONARYLISTSTORE, LgwDictionaryListStorePrivate))

G_END_DECLS

#include <libgwaei/dictionaryliststore-callbacks.h>
#include <libgwaei/dictionaryliststore-treemodel-interface.h>

#endif

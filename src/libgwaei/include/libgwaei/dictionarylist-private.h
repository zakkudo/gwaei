#ifndef LGW_DICTIONARYLIST_PRIVATE_INCLUDED
#define LGW_DICTIONARYLIST_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_DELETED,
  SIGNALID_INSERTED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

struct _Data {
  LgwDictionaryList *dictionary_list;
  GMenuModel *menumodel;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _LgwDictionaryListPrivate {
  struct _Data data;
};

struct _LgwDictionaryListClassPrivate {
  gint unused;
};

#define LGW_DICTIONARYLIST_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_DICTIONARYLIST, LgwDictionaryListPrivate))

G_END_DECLS

#include <libgwaei/dictionarylist-callbacks.h>
#include <libgwaei/dictionarylist-iter.h>

#endif

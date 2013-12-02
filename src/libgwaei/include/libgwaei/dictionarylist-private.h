#ifndef LGW_DICTIONARYLIST_PRIVATE_INCLUDED
#define LGW_DICTIONARYLIST_PRIVATE_INCLUDED

G_BEGIN_DECLS

/*
typedef enum {
  CLASS_SIGNALID_ROW_CHANGED,
  CLASS_SIGNALID_ROW_DELETED,
  CLASS_SIGNALID_ROW_HAS_CHILD_TOGGLED,
  CLASS_SIGNALID_ROW_INSERTED,
  CLASS_SIGNALID_ROWS_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;
*/

typedef enum {
  SIGNALID_ROW_CHANGED,
  SIGNALID_ROW_DELETED,
  SIGNALID_ROW_INSERTED,
  SIGNALID_ROWS_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

struct _Data {
  GMenuModel *menumodel;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _LgwDictionaryListPrivate {
  struct _Data data;
};

struct _LgwDictionaryListClassPrivate {
/*
  guint signalid[TOTAL_CLASS_SIGNALIDS];
*/
  gint UNUSED;

};

#define LGW_DICTIONARYLIST_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_DICTIONARYLIST, LgwDictionaryListPrivate))

G_END_DECLS

#include <libgwaei/dictionarylist-callbacks.h>
#include <libgwaei/dictionarylist-iter.h>

#endif

#ifndef LW_DICTIONARYLIST_PRIVATE_INCLUDED
#define LW_DICTIONARYLIST_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  CLASS_SIGNALID_CHANGED,
  CLASS_SIGNALID_INSERTED,
  CLASS_SIGNALID_DELETED,
  CLASS_SIGNALID_REORDERED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;


struct _LwDictionaryListPrivate {
  GList *list;
  GMutex mutex;
};

struct _LwDictionaryListClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];

  //Signals
  void (*row_changed) (LwDictionaryList* dictionarylist, gpointer data);
  void (*row_inserted) (LwDictionaryList* dictionarylist, gpointer data);
  void (*row_deleted) (LwDictionaryList* dictionarylist, gpointer data);
  void (*rows_reordered) (LwDictionaryList* dictionarylist, gpointer data);
};

#define LW_DICTIONARYLIST_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYLIST, LwDictionaryListPrivate));

G_END_DECLS

#endif



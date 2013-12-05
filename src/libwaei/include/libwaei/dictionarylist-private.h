#ifndef LW_DICTIONARYLIST_PRIVATE_INCLUDED
#define LW_DICTIONARYLIST_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_INSERTED,
  SIGNALID_DELETED,
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


struct _LwDictionaryListPrivate {
  GList *list;
  GMutex mutex;
  GMenuModel *menu_model;
  guint signalid[TOTAL_SIGNALIDS];
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

void lw_dictionarylist_menumodel_insert (LwDictionaryList *dictioanry_list, LwDictionary *dictionary, gint index_);
void lw_dictionarylist_menumodel_append (LwDictionaryList *dictionary_list, LwDictionary *dictionary);
void lw_dictionarylist_sync_menumodel (LwDictionaryList *dictionary_list);

G_END_DECLS

#include <libwaei/dictionarylist-callbacks.h>

#endif



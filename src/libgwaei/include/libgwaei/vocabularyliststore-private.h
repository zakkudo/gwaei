#ifndef LGW_VOCABULARYLISTSTORE_PRIVATE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_PRIVATE_INCLUDED

G_BEGIN_DECLS


typedef enum {
  SIGNALID_INTERNAL_ORDER_CHANGED,
  TOTAL_SIGNALIDS
} SignalId;

typedef enum {
  PROP_0,
  PROP_PREFERENCES,
  PROP_ORDER,
  TOTAL_PROPS
} Prop;

struct _Index {
  GHashTable *filename;
  GHashTable *wordstore;
  GHashTable *update_filename_callback;
  GHashTable *changed_callback;
};

struct _Data {
  gint list_new_index;
  GList *list; //!< LgwVocabularyWordStore
  GMenuModel *menumodel;
  gboolean has_changes;
  gint length;
  struct _Index index;
  GList **array;
  gchar **order;
  guint signalid[TOTAL_SIGNALIDS];
  gboolean loaded;
};


struct _Config {
  LwPreferences *preferences;
  gint sort_column_id;
  GtkSortType order;
  GtkTreeIterCompareFunc sort_func[LGW_VOCABULARYLISTSTORE_COLUMN_NAME];
  GtkTreeIterCompareFunc default_sort_func;
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

//In general, the methods using indices should not be exposed publicly
LgwVocabularyWordStore* lgw_vocabularyliststore_nth (LgwVocabularyListStore *self, gint position);
GList* lgw_vocabularyliststore_remove_by_indices (LgwVocabularyListStore *self, gint *indices);
GList* lgw_vocabularyliststore_delete_by_indices (LgwVocabularyListStore *self, gint *indices);

gint* lgw_vocabularyliststore_tree_paths_to_indices (LgwVocabularyListStore *self, GList *tree_paths);
GList* lgw_vocabularyliststore_indices_to_tree_paths (LgwVocabularyListStore *self, gint *indices);

gchar** lgw_vocabularyliststore_get_actual_order (LgwVocabularyListStore *self);
void lgw_vocabularyliststore_sync_previous_indices (LgwVocabularyListStore *self);

G_END_DECLS

#include <libgwaei/vocabularyliststore-callbacks.h>
#include <libgwaei/vocabularyliststore-treemodel-interface.h>
#include <libgwaei/vocabularyliststore-treedragdest-interface.h>
#include <libgwaei/vocabularyliststore-treedragsource-interface.h>
#include <libgwaei/vocabularyliststore-treesortable-interface.h>

#endif


#ifndef LGW_VOCABULARYWORDSTORE_PRIVATE_INCLUDED
#define LGW_VOCABULARYWORDSTORE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  SIGNALID_CHANGED,
  SIGNALID_DELETED,
  SIGNALID_INSERTED,
  SIGNALID_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;


struct _Data {
  gint list_new_index;
  GList *list; //!< LgwVocabularyWordStore
  guint signalid[TOTAL_SIGNALIDS];
};


struct _Config {
  gint sort_column_id;
  GtkSortType order;
  GtkTreeIterCompareFunc sort_func[TOTAL_LW_WORD_FIELDS];
  GtkTreeIterCompareFunc default_sort_func;
};

struct _LgwVocabularyWordStorePrivate {
  struct _Data data;
  struct _Config config;
};

#define LGW_VOCABULARYWORDSTORE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStorePrivate))

gint* lgw_vocabulary_wordstore_tree_paths_to_indices (LgwVocabularyWordStore *self, GList *tree_paths);
GList* lgw_vocabularywordstore_indices_to_tree_paths (LgwVocabularyWordStore *self, gint* indices);

G_END_DECLS

#include <libgwaei/vocabularywordstore-callbacks.h>
#include <libgwaei/vocabularywordstore-treemodel-interface.h>
#include <libgwaei/vocabularywordstore-treedragsource-interface.h>
#include <libgwaei/vocabularywordstore-treedragdest-interface.h>
#include <libgwaei/vocabularywordstore-treesortable-interface.h>

#endif


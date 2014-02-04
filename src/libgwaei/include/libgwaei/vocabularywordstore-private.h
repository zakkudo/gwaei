#ifndef LGW_VOCABULARYWORDSTORE_PRIVATE_INCLUDED
#define LGW_VOCABULARYWORDSTORE_PRIVATE_INCLUDED

G_BEGIN_DECLS


struct _Data {
  gint list_new_index;
  GList *list; //!< LgwVocabularyWordStore
  gint length;
};

struct _LgwVocabularyWordStorePrivate {
  struct _Data data;
};

struct _LgwVocabularyWordStoreClassPrivate {
  gint unused;
};

#define LGW_VOCABULARYWORDSTORE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStorePrivate))

G_END_DECLS

#include <libgwaei/vocabularywordstore-treemodel-interface.h>
#include <libgwaei/vocabularywordstore-callbacks.h>

#endif


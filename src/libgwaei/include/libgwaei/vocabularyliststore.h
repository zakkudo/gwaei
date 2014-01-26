#ifndef LGW_VOCABULARYLISTSTORE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_INCLUDED

#include <libgwaei/vocabularywordstore.h>

G_BEGIN_DECLS

typedef enum { 
  LGW_VOCABULARYLISTSTORE_COLUMN_NAME,
  LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME,
  LGW_VOCABULARYLISTSTORE_COLUMN_HAS_CHANGES,
  LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT,
  TOTAL_LGW_VOCABULARYLISTSTORE_COLUMNS
} LgwVocabularyListStoreColumn;

//Boilerplate
typedef struct _LgwVocabularyListStore LgwVocabularyListStore;
typedef struct _LgwVocabularyListStoreClass LgwVocabularyListStoreClass;
typedef struct _LgwVocabularyListStorePrivate LgwVocabularyListStorePrivate;
typedef struct _LgwVocabularyListStoreClassPrivate LgwVocabularyListStoreClassPrivate;

typedef struct _LgwVocabualryListStoreRow LgwVocabularyListStoreRow;

#define LGW_TYPE_VOCABULARYLISTSTORE              (lgw_vocabularyliststore_get_type())
#define LGW_VOCABULARYLISTSTORE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStore))
#define LGW_VOCABULARYLISTSTORE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStoreClass))
#define LGW_IS_VOCABULARYLISTSTORE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_VOCABULARYLISTSTORE))
#define LGW_IS_VOCABULARYLISTSTORE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_VOCABULARYLISTSTORE))
#define LGW_VOCABULARYLISTSTORE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStoreClass))

struct _LgwVocabularyListStore {
  GObject object;
  LgwVocabularyListStorePrivate *priv;
};

struct _LgwVocabularyListStoreClass {
  GtkListStoreClass parent_class;
  LgwVocabularyListStoreClassPrivate *priv;

  //Signals
  void (*row_changed) (LwDictionaryList* dictionarylist, gint position, gpointer data);
  void (*row_inserted) (LwDictionaryList* dictionarylist, gint position, gpointer data);
  void (*row_deleted) (LwDictionaryList* dictionarylist, gint position, gpointer data);
  void (*rows_reordered) (LwDictionaryList* dictionarylist, gint *order, gpointer data);
};

//Methods
LgwVocabularyListStore* lgw_vocabularyliststore_new (void);
GType lgw_vocabularyliststore_get_type (void) G_GNUC_CONST;

void lgw_vocabularyliststore_add (LgwVocabularyListStore *vocabulary_list_store, const gchar* FILENAME);
void lgw_vocabularyliststore_remove_by_position (LgwVocabularyListStore *vocabulary_list_store, gint position);

G_END_DECLS

#endif

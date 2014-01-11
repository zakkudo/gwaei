#ifndef LGW_VOCABULARYWORDSTORE_INCLUDED
#define LGW_VOCABULARYWORDSTORE_INCLUDED

G_BEGIN_DECLS


typedef enum { 
  LGW_VOCABULARYWORDSTORE_COLUMN_POSITION_INTEGER,
  LGW_VOCABULARYWORDSTORE_COLUMN_POSITION_STRING,
  LGW_VOCABULARYWORDSTORE_COLUMN_KANJI,
  LGW_VOCABULARYWORDSTORE_COLUMN_FURIGANA,
  LGW_VOCABULARYWORDSTORE_COLUMN_DEFINITIONS,
  LGW_VOCABULARYWORDSTORE_COLUMN_CORRECT_GUESSES,
  LGW_VOCABULARYWORDSTORE_COLUMN_INCORRECT_GUESSES,
  LGW_VOCABULARYWORDSTORE_COLUMN_SCORE,
  LGW_VOCABULARYWORDSTORE_COLUMN_TIMESTAMP,
  LGW_VOCABULARYWORDSTORE_COLUMN_DAYS,
  LGW_VOCABULARYWORDSTORE_COLUMN_WEIGHT,
  TOTAL_GW_VOCABULARYWORDSTORE_COLUMNS
} LgwVocabularyWordStoreColumn;

//Boilerplate
typedef struct _LgwVocabularyWordStore LgwVocabularyWordStore;
typedef struct _LgwVocabularyWordStoreClass LgwVocabularyWordStoreClass;
typedef struct _LgwVocabularyWordStorePrivate LgwVocabularyWordStorePrivate;

#define LGW_TYPE_VOCABULARYWORDSTORE              (lgw_vocabularywordstore_get_type())
#define LGW_VOCABULARYWORDSTORE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStore))
#define LGW_VOCABULARYWORDSTORE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStoreClass))
#define LGW_IS_VOCABULARYWORDSTORE(obj)                    (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_VOCABULARYWORDSTORE))
#define LGW_IS_VOCABULARYWORDSTORE_CLASS(klass)            (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_VOCABULARYWORDSTORE))
#define LGW_VOCABULARYWORDSTORE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStoreClass))

struct _LgwVocabularyWordStore {
  GtkListStore model;
  LgwVocabularyWordStorePrivate *priv;
};

struct _LgwVocabularyWordStoreClass {
  GtkListStoreClass parent_class;
  void (*changed) (LgwVocabularyWordStore *store);
};

//Methods
LgwVocabularyWordStore* lgw_vocabularywordstore_new (const gchar*);
GType lgw_vocabularywordstore_get_type (void) G_GNUC_CONST;


G_END_DECLS

#endif

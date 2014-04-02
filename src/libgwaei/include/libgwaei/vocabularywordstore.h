#ifndef LGW_VOCABULARYWORDSTORE_INCLUDED
#define LGW_VOCABULARYWORDSTORE_INCLUDED

G_BEGIN_DECLS


//The first items MUST be in the same order as LwWordField
typedef enum { 
  //Settable using lgw_vocabularywordstore_set
  LGW_VOCABULARYWORDSTORE_COLUMN_KANJI,              //LW_WORD_FIELD_KANJI
  LGW_VOCABULARYWORDSTORE_COLUMN_READING,            //LW_WORD_FIELD_READING
  LGW_VOCABULARYWORDSTORE_COLUMN_DEFINITION,         //LW_WORD_FIELD_DEFINITION

  //Not settable
  LGW_VOCABULARYWORDSTORE_COLUMN_CORRECT_GUESSES,    //LW_WORD_FIELD_CORRECT_GUESSES
  LGW_VOCABULARYWORDSTORE_COLUMN_INCORRECT_GUESSES,  //LW_WORD_FIELD_INCORRECT_GUESSES
  LGW_VOCABULARYWORDSTORE_COLUMN_LAST_STUDIED,       //LW_WORD_FIELD_TIMESTAMP
  LGW_VOCABULARYWORDSTORE_COLUMN_SCORE,
  LGW_VOCABULARYWORDSTORE_COLUMN_WEIGHT,
  LGW_VOCABULARYWORDSTORE_COLUMN_WORD,
  LGW_VOCABULARYWORDSTORE_COLUMN_SAVED_POSITION,
  LGW_VOCABULARYWORDSTORE_COLUMN_POSITION,
  TOTAL_LGW_VOCABULARYWORDSTORE_COLUMNS
} LgwVocabularyWordStoreColumn;

//Boilerplate
typedef struct _LgwVocabularyWordStore LgwVocabularyWordStore;
typedef struct _LgwVocabularyWordStoreClass LgwVocabularyWordStoreClass;
typedef struct _LgwVocabularyWordStorePrivate LgwVocabularyWordStorePrivate;
typedef struct _LgwVocabularyWordStoreClassPrivate LgwVocabularyWordStoreClassPrivate;

#define LGW_TYPE_VOCABULARYWORDSTORE              (lgw_vocabularywordstore_get_type())
#define LGW_VOCABULARYWORDSTORE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStore))
#define LGW_VOCABULARYWORDSTORE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStoreClass))
#define LGW_IS_VOCABULARYWORDSTORE(obj)                    (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_VOCABULARYWORDSTORE))
#define LGW_IS_VOCABULARYWORDSTORE_CLASS(klass)            (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_VOCABULARYWORDSTORE))
#define LGW_VOCABULARYWORDSTORE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_VOCABULARYWORDSTORE, LgwVocabularyWordStoreClass))

struct _LgwVocabularyWordStore {
  LwVocabulary vocabulary;
  LgwVocabularyWordStorePrivate *priv;
};

struct _LgwVocabularyWordStoreClass {
  LwVocabularyClass parent_class;
};

//Methods
LgwVocabularyWordStore* lgw_vocabularywordstore_new (const gchar* FILENAME);
LgwVocabularyWordStore* lgw_vocabularywordstore_new_from_wordstores (GList *wordstores, LwProgressCallback cb);
GType lgw_vocabularywordstore_get_type (void) G_GNUC_CONST;

LwWord* lgw_vocabularywordstore_get_word (LgwVocabularyWordStore *self, GtkTreePath *tree_path);
GtkTreePath* lgw_vocabularywordstore_find_by_word (LgwVocabularyWordStore *self, LwWord *word);

GList* lgw_vocabularywordstore_get_tree_paths (LgwVocabularyWordStore *self, GList *words);

GList* lgw_vocabularywordstore_insert (LgwVocabularyWordStore *self, GtkTreePath *tree_path, GList *words);
GList* lgw_vocabularywordstore_remove (LgwVocabularyWordStore *self, GList *tree_path_list);

void lgw_vocabularywordstore_set (LgwVocabularyWordStore *self, GtkTreePath *tree_path, ...);

LgwVocabularyWordStore* lgw_vocabularywordstore_copy (LgwVocabularyWordStore *self);

void lgw_vocabularywordstore_sort (LgwVocabularyWordStore *vocabulary_word_store);

G_END_DECLS

#endif

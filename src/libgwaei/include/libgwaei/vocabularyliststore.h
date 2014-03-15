#ifndef LGW_VOCABULARYLISTSTORE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_INCLUDED

#include <libgwaei/vocabularywordstore.h>

G_BEGIN_DECLS


typedef enum { 
  //Setable using lgw_vocabularyliststore_set
  LGW_VOCABULARYLISTSTORE_COLUMN_FILENAME,

  //Not settable
  LGW_VOCABULARYLISTSTORE_COLUMN_NAME,
  LGW_VOCABULARYLISTSTORE_COLUMN_STYLE_WEIGHT,
  LGW_VOCABULARYLISTSTORE_COLUMN_TOTAL_WORDS,
  LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT,
  LGW_VOCABULARYLISTSTORE_COLUMN_POSITION,
  LGW_VOCABULARYLISTSTORE_COLUMN_SAVED_POSITION,
  TOTAL_LGW_VOCABULARYLISTSTORE_COLUMNS
} LgwVocabularyListStoreColumn;

//Boilerplate
typedef struct _LgwVocabularyListStore LgwVocabularyListStore;
typedef struct _LgwVocabularyListStoreClass LgwVocabularyListStoreClass;
typedef struct _LgwVocabularyListStorePrivate LgwVocabularyListStorePrivate;
typedef struct _LgwVocabularyListStoreClassPrivate LgwVocabularyListStoreClassPrivate;

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
};

//Methods
LgwVocabularyListStore* lgw_vocabularyliststore_new (LwPreferences *preferences);
GType lgw_vocabularyliststore_get_type (void) G_GNUC_CONST;

GList* lgw_vocabularyliststore_insert (LgwVocabularyListStore *self, GtkTreePath *tree_path, GList *wordstorelist);
GList* lgw_vocabularyliststore_remove (LgwVocabularyListStore *self, GList *tree_path_list);
GList* lgw_vocabularyliststore_delete (LgwVocabularyListStore *self, GList *tree_path_list);
void lgw_vocabularyliststore_clear (LgwVocabularyListStore *self);

LwPreferences* lgw_vocabularyliststore_get_preferences (LgwVocabularyListStore *self);
void lgw_vocabularyliststore_set_preferences (LgwVocabularyListStore *self, LwPreferences *preferences);

void lgw_vocabularyliststore_load (LgwVocabularyListStore *self);

gint lgw_vocabularyliststore_length (LgwVocabularyListStore *self);

LgwVocabularyWordStore* lgw_vocabularyliststore_get_wordstore (LgwVocabularyListStore *self, GtkTreePath *tree_path);
GtkTreePath* lgw_vocabularyliststore_find_by_wordstore (LgwVocabularyListStore *self, LgwVocabularyWordStore *vocabulary_word_store);
GtkTreePath* lgw_vocabularyliststore_find_by_filename (LgwVocabularyListStore *self, const gchar *FILENAME);

GList* lgw_vocabularyliststore_get_tree_paths (LgwVocabularyListStore *self, GList *wordstores);

void lgw_vocabularyliststore_set (LgwVocabularyListStore *self, GtkTreePath *tree_path, ...);
void lgw_vocabularyliststore_set_valist (LgwVocabularyListStore *self, GtkTreePath *tree_path, va_list va);

G_END_DECLS

#endif

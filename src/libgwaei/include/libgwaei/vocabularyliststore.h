#ifndef LGW_VOCABULARYLISTSTORE_INCLUDED
#define LGW_VOCABULARYLISTSTORE_INCLUDED

#include <libgwaei/vocabularywordstore.h>

G_BEGIN_DECLS

typedef enum {
  LGW_VOCABULARYLISTSTORE_CLASS_SIGNALID_CHANGED,
  TOTAL_LGW_VOCABULARYLISTSTORE_CLASS_SIGNALIDS
} LgwVocabularyListStoreClassSignalId;

typedef enum { 
  LGW_VOCABULARYLISTSTORE_COLUMN_NAME,
  LGW_VOCABULARYLISTSTORE_COLUMN_CHANGED,
  LGW_VOCABULARYLISTSTORE_COLUMN_OBJECT,
  TOTAL_LGW_VOCABULARYLISTSTORE_COLUMNS
} LgwVocabularyListStoreColumn;

//Boilerplate
typedef struct _LgwVocabularyListStore LgwVocabularyListStore;
typedef struct _LgwVocabularyListStoreClass LgwVocabularyListStoreClass;
typedef struct _LgwVocabularyListStorePrivate LgwVocabularyListStorePrivate;

#define LGW_TYPE_VOCABULARYLISTSTORE              (lgw_vocabularyliststore_get_type())
#define LGW_VOCABULARYLISTSTORE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStore))
#define LGW_VOCABULARYLISTSTORE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStoreClass))
#define LGW_IS_VOCABULARYLISTSTORE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_VOCABULARYLISTSTORE))
#define LGW_IS_VOCABULARYLISTSTORE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_VOCABULARYLISTSTORE))
#define LGW_VOCABULARYLISTSTORE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_VOCABULARYLISTSTORE, LgwVocabularyListStoreClass))

struct _LgwVocabularyListStore {
  GtkListStore store;
  LgwVocabularyListStorePrivate *priv;
};

struct _LgwVocabularyListStoreClass {
  GtkListStoreClass parent_class;
  guint signalid[TOTAL_LGW_VOCABULARYLISTSTORE_CLASS_SIGNALIDS];
  void (*changed) (LgwVocabularyListStore *store);
};

//Methods
GtkListStore* lgw_vocabularyliststore_new (void);
GType lgw_vocabularyliststore_get_type (void) G_GNUC_CONST;

void lgw_vocabularyliststore_save (LgwVocabularyListStore*, GtkTreeIter*);
void lgw_vocabularyliststore_save_all (LgwVocabularyListStore*);

void lgw_vocabularyliststore_revert (LgwVocabularyListStore*, GtkTreeIter*);
void lgw_vocabularyliststore_revert_all (LgwVocabularyListStore*);

void lgw_vocabularyliststore_save_list_order (LgwVocabularyListStore*, LwPreferences*);
void lgw_vocabularyliststore_load_list_order (LgwVocabularyListStore*, LwPreferences*);

GtkListStore* lgw_vocabularyliststore_get_wordstore_by_iter (LgwVocabularyListStore*, GtkTreeIter*);
GtkListStore* lgw_vocabularyliststore_get_wordstore_by_index (LgwVocabularyListStore*, gint);
GtkListStore* lgw_vocabularyliststore_get_wordstore_by_name (LgwVocabularyListStore*, const gchar*);
gchar* lgw_vocabularyliststore_get_name_by_iter (LgwVocabularyListStore*, GtkTreeIter*);

void lgw_vocabularyliststore_remove_path_list (LgwVocabularyListStore*, GList*);
void lgw_vocabularyliststore_new_list (LgwVocabularyListStore*, GtkTreeIter*);
gboolean lgw_vocabularyliststore_list_exists (LgwVocabularyListStore*, const gchar*);
gboolean lgw_vocabularyliststore_has_changes (LgwVocabularyListStore*);
void lgw_vocabularyliststore_set_has_changes (LgwVocabularyListStore*, gboolean);
GMenuModel* lgw_vocabularyliststore_get_menumodel (LgwVocabularyListStore*);

void lgw_vocabularyliststore_load (LgwVocabularyListStore*);

G_END_DECLS

#endif

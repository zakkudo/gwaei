#ifndef LGW_DICTIONARYLISTSTORE_INCLUDED
#define LGW_DICTIONARYLISTSTORE_INCLUDED

G_BEGIN_DECLS


typedef enum { 
  LGW_DICTIONARYLISTSTORE_COLUMN_ICON_NAME,    //G_TYPE_STRING
  LGW_DICTIONARYLISTSTORE_COLUMN_POSITION,     //G_TYPE_STRING
  LGW_DICTIONARYLISTSTORE_COLUMN_NAME,         //G_TYPE_STRING
  LGW_DICTIONARYLISTSTORE_COLUMN_LONG_NAME,    //G_TYPE_STRING
  LGW_DICTIONARYLISTSTORE_COLUMN_ENGINE,       //G_TYPE_STRING
  LGW_DICTIONARYLISTSTORE_COLUMN_SHORTCUT,     //G_TYPE_STRING
  LGW_DICTIONARYLISTSTORE_COLUMN_SELECTED,     //G_TYPE_BOOLEAN
  LGW_DICTIONARYLISTSTORE_COLUMN_DICTIONARY,   //G_TYPE_POINTER
  TOTAL_LGW_DICTIONARYLISTSTORE_COLUMNS
} LgwDictionaryListStoreColumns;


//Boilerplate
typedef struct _LgwDictionaryListStore LgwDictionaryListStore;
typedef struct _LgwDictionaryListStoreClass LgwDictionaryListStoreClass;
typedef struct _LgwDictionaryListStorePrivate LgwDictionaryListStorePrivate;
typedef struct _LgwDictionaryListStoreClassPrivate LgwDictionaryListStoreClassPrivate;

#define LGW_TYPE_DICTIONARYLISTSTORE              (lgw_dictionaryliststore_get_type())
#define LGW_DICTIONARYLISTSTORE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_DICTIONARYLISTSTORE, LgwDictionaryListStore))
#define LGW_DICTIONARYLISTSTORE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_DICTIONARYLISTSTORE, LgwDictionaryListStoreClass))
#define LGW_IS_DICTIONARYLISTSTORE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_DICTIONARYLISTSTORE))
#define LGW_IS_DICTIONARYLISTSTORE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_DICTIONARYLISTSTORE))
#define LGW_DICTIONARYLISTSTORE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_DICTIONARYLISTSTORE, LgwDictionaryListStoreClass))

struct _LgwDictionaryListStore {
  LwDictionaryList list_store;
  LgwDictionaryListStorePrivate *priv;
};

struct _LgwDictionaryListStoreClass {
  LwDictionaryListClass parent_class;
  LgwDictionaryListStoreClassPrivate *priv;
};

//Methods
LgwDictionaryListStore* lgw_dictionaryliststore_new (LwPreferences *preferences);
GType lgw_dictionaryliststore_get_type (void) G_GNUC_CONST;

void lgw_dictionaryliststore_sync_menumodel (LgwDictionaryListStore *dictionary_list_store);
GMenuModel* lgw_dictionaryliststore_get_menumodel (LgwDictionaryListStore *dictionary_list_store);

G_END_DECLS

#endif

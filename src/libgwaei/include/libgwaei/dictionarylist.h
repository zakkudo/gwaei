#ifndef LGW_DICTIONARYLIST_INCLUDED
#define LGW_DICTIONARYLIST_INCLUDED

G_BEGIN_DECLS

/*
typedef enum {
  TOTAL_LGW_DICTIONARYLIST_SIGNALIDS
} LgwDictionaryListSignalId;
*/


typedef enum { 
  LGW_DICTIONARYLIST_COLUMN_IMAGE,
  LGW_DICTIONARYLIST_COLUMN_POSITION,
  LGW_DICTIONARYLIST_COLUMN_NAME, 
  LGW_DICTIONARYLIST_COLUMN_LONG_NAME, 
  LGW_DICTIONARYLIST_COLUMN_ENGINE,
  LGW_DICTIONARYLIST_COLUMN_SHORTCUT,
  LGW_DICTIONARYLIST_COLUMN_SELECTED,
  LGW_DICTIONARYLIST_COLUMN_DICT_POINTER,
  TOTAL_LGW_DICTIONARYLIST_COLUMNS
} LgwDictionaryListColumns;


//Boilerplate
typedef struct _LgwDictionaryList LgwDictionaryList;
typedef struct _LgwDictionaryListClass LgwDictionaryListClass;
typedef struct _LgwDictionaryListPrivate LgwDictionaryListPrivate;

#define LGW_TYPE_DICTIONARYLIST              (lgw_dictionarylist_get_type())
#define LGW_DICTIONARYLIST(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_DICTIONARYLIST, LgwDictionaryList))
#define LGW_DICTIONARYLIST_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_DICTIONARYLIST, LgwDictionaryListClass))
#define LGW_IS_DICTIONARYLIST(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_DICTIONARYLIST))
#define LGW_IS_DICTIONARYLIST_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_DICTIONARYLIST))
#define LGW_DICTIONARYLIST_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_DICTIONARYLIST, LgwDictionaryListClass))

struct _LgwDictionaryList {
  LwDictionaryList list;
  LgwDictionaryListPrivate *priv;
};

struct _LgwDictionaryListClass {
  LwDictionaryListClass parent_class;
//  guint signalid[TOTAL_LGW_DICTIONARYLIST_SIGNALIDS];
};

//Methods
LgwDictionaryList* lgw_dictionarylist_new (void);
GType lgw_dictionarylist_get_type (void) G_GNUC_CONST;

void lgw_dictionarylist_reload (LgwDictionaryList*, LwPreferences*);
LwDictionaryList* lgw_dictionarylist_get_dictionarylist (LgwDictionaryList*);
void lgw_dictionarylist_save_order (LgwDictionaryList*, LwPreferences*);
void lgw_dictionarylist_load_order (LgwDictionaryList*, LwPreferences*);
void lgw_dictionarylist_update (LgwDictionaryList*);
void lgw_dictionarylist_normalize (LgwDictionaryList*);
void lgw_dictionarylist_sync_menumodel (LgwDictionaryList*);
GMenuModel* lgw_dictionarylist_get_menumodel (LgwDictionaryList*);
GtkListStore* lgw_dictionarylist_get_liststore (LgwDictionaryList*);
void lgw_dictionarylist_sync_treestore (LgwDictionaryList*);

G_END_DECLS

#endif

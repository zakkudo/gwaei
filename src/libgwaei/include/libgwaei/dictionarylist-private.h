#ifndef LGW_DICTIONARYLIST_PRIVATE_INCLUDED
#define LGW_DICTIONARYLIST_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _Data {
  GtkListStore *liststore;
  GMenuModel *menumodel;
};

struct _LgwDictionaryListPrivate {
  struct _Data data;
};

#define LGW_DICTIONARYLIST_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_DICTIONARYLIST, LgwDictionaryListPrivate))

G_END_DECLS

#endif

#ifndef LGW_DICTIONARYLISTVIEW_PRIVATE_INCLUDED
#define LGW_DICTIONARYLISTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
    GtkBox *box;
    GtkTreeView *tree_view;
    GtkToolbar *toolbar;
    GtkScrolledWindow *scrolled_window;
};

struct _Config {
    gboolean readonly;
    gboolean show_toolbar;
    gboolean show_icons;
};

struct _Data {
    LgwDictionaryList *dictionary_list;
};

struct _LgwDictionaryListViewPrivate {
    struct _UI ui;
    struct _Config config;
    struct _Data data;
};

#define LGW_DICTIONARYLISTVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_DICTIONARYLISTVIEW, LgwDictionaryListViewPrivate));

G_END_DECLS

#endif

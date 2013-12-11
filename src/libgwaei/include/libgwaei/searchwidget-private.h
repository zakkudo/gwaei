#ifndef LGW_SEARCHWIDGET_PRIVATE_INCLUDED
#define LGW_SEARCHWIDGET_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
    GtkBox *box;
    GtkPaned *paned;
    LgwSearchEntry *search_entry;
    GtkSearchBar *search_bar;
    LgwResultsView *results_view;
    LgwDictionaryListBox *dictionary_list_box;
};

struct _Data {
    GMenuModel *button_menu_model;
    GMenuModel *window_menu_model;
    LgwActionGroup *action_group;
    GList *action_group_list;
};

struct _LgwSearchWidgetPrivate {
    struct _UI ui;
    struct _Data data;
};

#define LGW_SEARCHWIDGET_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_SEARCHWIDGET, LgwSearchWidgetPrivate));

G_END_DECLS

#endif

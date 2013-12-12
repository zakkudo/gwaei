#ifndef LGW_SEARCHWIDGET_PRIVATE_INCLUDED
#define LGW_SEARCHWIDGET_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_ACTIONS,
    TOTAL_PROPS
} Props;

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

static GList* lgw_searchwidget_get_actions (LgwActionable *actionable);
static void lgw_searchwidget_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);
static void lgw_searchwidget_sync_actions (LgwSearchWidget *search_widget);

G_END_DECLS

#endif

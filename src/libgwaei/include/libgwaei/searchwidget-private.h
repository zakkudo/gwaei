#ifndef LGW_SEARCHWIDGET_PRIVATE_INCLUDED
#define LGW_SEARCHWIDGET_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    SIGNALID_SEARCHENTRY_ACTIONS,
    SIGNALID_RESULTSVIEW_ACTIONS,
    TOTAL_SIGNALIDS
} SignalId;

typedef enum {
    PROP_0,
    PROP_ACTIONS,
    PROP_PREFERENCES,
    PROP_DICTIONARYLISTSTORE,
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
    guint signalid[TOTAL_SIGNALIDS];
    LgwDictionaryListStore *dictionary_list_store;
};

struct _Config {
    LwPreferences *preferences;
};

struct _LgwSearchWidgetPrivate {
    struct _UI ui;
    struct _Data data;
    struct _Config config;
};

struct _LgwSearchWidgetClassPrivate {
    GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_SEARCHWIDGET_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_SEARCHWIDGET, LgwSearchWidgetPrivate));

static GMenuModel* lgw_searchwidget_get_button_menu_model (LgwMenuable *widget);
static GMenuModel* lgw_searchwidget_get_window_menu_model (LgwMenuable *widget);

static void lgw_searchwidget_init_menuable_interface (LgwMenuableInterface *iface);
static void lgw_searchwidget_init_actionable_interface (LgwActionableInterface *iface);

static GList* lgw_searchwidget_get_actions (LgwActionable *actionable);
static void lgw_searchwidget_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);
void lgw_searchwidget_sync_actions (LgwSearchWidget *search_widget);

G_END_DECLS

#include <libgwaei/searchwidget-callbacks.h>

#endif

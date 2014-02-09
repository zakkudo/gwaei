#ifndef LGW_VOCABULARYWIDGET_PRIVATE_INCLUDED
#define LGW_VOCABULARYWIDGET_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    SIGNALID_VOCABULARYENTRY_ACTIONS,
    SIGNALID_RESULTSVIEW_ACTIONS,
    TOTAL_SIGNALIDS
} SignalId;

typedef enum {
    PROP_0,
    PROP_ACTIONS,
    PROP_PREFERENCES,
    PROP_VOCABULARYLISTSTORE,
    TOTAL_PROPS
} Props;

struct _UI {
    GtkBox *box;
    GtkPaned *paned;
    LgwVocabularyListView *vocabulary_list_view;
    LgwVocabularyWordView *vocabulary_word_view;
};

struct _Data {
    GMenuModel *button_menu_model;
    GMenuModel *window_menu_model;
    LgwActionGroup *action_group;
    GList *action_group_list;
    guint signalid[TOTAL_SIGNALIDS];
    LgwVocabularyListStore *vocabulary_list_store;
};

struct _Config {
    LwPreferences *preferences;
};

struct _LgwVocabularyWidgetPrivate {
    struct _UI ui;
    struct _Data data;
    struct _Config config;
};

struct _LgwVocabularyWidgetClassPrivate {
    GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_VOCABULARYWIDGET_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYWIDGET, LgwVocabularyWidgetPrivate));

static GMenuModel* lgw_vocabularywidget_get_button_menu_model (LgwMenuable *widget);
static GMenuModel* lgw_vocabularywidget_get_window_menu_model (LgwMenuable *widget);

static void lgw_vocabularywidget_init_menuable_interface (LgwMenuableInterface *iface);

G_END_DECLS

#include <libgwaei/vocabularywidget-callbacks.h>

#endif

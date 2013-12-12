#ifndef LGW_SEARCHENTRY_PRIVATE_INCLUDED
#define LGW_SEARCHENTRY_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_SEARCH_AS_YOU_TYPE,
    PROP_ACTIONS,
    TOTAL_PROPS
} Props;

typedef enum {
    SIGNALID_ACTIVATED,
    SIGNALID_CHANGED,
    TOTAL_SIGNALIDS 
} SignalIds;

struct _UI {
    GtkBox *box;
    GtkSearchEntry *search_entry;
};

struct _Config {
    gboolean search_as_you_type;
    gboolean spellcheck;
};

struct _Data {
    guint signalid[TOTAL_SIGNALIDS];
    LgwDictionaryListBox *dictionary_list_box;
    LgwResultsView *results_view;
    GList *action_group_list;
    LgwActionGroup *action_group;
};

struct _LgwSearchEntryPrivate {
    struct _UI ui;
    struct _Config config;
    struct _Data data;
};

struct _LgwSearchEntryClassPrivate {
    GParamSpec *param_spec[TOTAL_PROPS];
};

#define LGW_SEARCHENTRY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_SEARCHENTRY, LgwSearchEntryPrivate));

static GList* lgw_searchentry_get_actions (LgwActionable *actionable);
static void lgw_searchentry_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);
static void lgw_searchentry_sync_actions (LgwSearchEntry *search_entry);

G_END_DECLS

#include <libgwaei/searchentry-callbacks.h>

#endif

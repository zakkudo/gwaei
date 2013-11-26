#ifndef LGW_SEARCHENTRY_PRIVATE_INCLUDED
#define LGW_SEARCHENTRY_PRIVATE_INCLUDED

G_BEGIN_DECLS

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
};

struct _LgwSearchEntryPrivate {
    struct _UI ui;
    struct _Config config;
    struct _Data data;
};

#define LGW_SEARCHENTRY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_SEARCHENTRY, LgwSearchEntryPrivate));

G_END_DECLS

#include <libgwaei/searchentry-callbacks.h>

#endif

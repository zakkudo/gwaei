#ifndef LGW_SEARCHENTRY_PRIVATE_INCLUDED
#define LGW_SEARCHENTRY_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
    GtkBox *box;
    GtkSearchEntry *search_entry;
};

struct _Config {
    gboolean search_as_you_type;
    gboolean spellcheck;
};

struct _LgwSearchEntryPrivate {
    struct _UI ui;
    struct _Config config;
};

#define LGW_SEARCHENTRY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_SEARCHENTRY, LgwSearchEntryPrivate));

G_END_DECLS

#endif

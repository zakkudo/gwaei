#ifndef LGW_DICTIONARYLISTBOX_PRIVATE_INCLUDED
#define LGW_DICTIONARYLISTBOX_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_DICTIONARYLISTSTORE,
  PROP_EDITABLE,
  TOTAL_PROPS
} Props;

typedef enum {
  UNUSED_SIGNALID,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;


struct _UI {
    GtkBox *box;
    GtkListBox *list_box;
    GtkToolbar *toolbar;
    GtkScrolledWindow *scrolled_window;
};

struct _Config {
    gboolean show_toolbar;
    gboolean show_icons;
    gboolean editable;
};

struct _Data {
    LgwDictionaryListStore *dictionary_list_store;
};

struct _LgwDictionaryListBoxPrivate {
    struct _UI ui;
    struct _Config config;
    struct _Data data;
};

struct _LgwDictionaryListBoxClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_DICTIONARYLISTBOX_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_DICTIONARYLISTBOX, LgwDictionaryListBoxPrivate));

G_END_DECLS

#endif

#ifndef LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED
#define LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_ACTIONS,
    TOTAL_PROPS
} Props;

typedef enum {
  TOTAL_SIGNALIDS
} SignalId;

struct _UI {
  GtkBox *box;
  GtkTextView *text_view;
  GtkTextBuffer *text_buffer;
  GtkScrolledWindow *scrolled_window;
};

struct _Config {
  gint scale_percent;
  gint max_results;
};

struct _Data {
  guint signalid[TOTAL_SIGNALIDS];
  LwSearch *search;
  GList *searchlist;
  GList *searchiteratorlist;
  guint timeoutid;
};

struct _LgwResultsTextViewPrivate {
  struct _UI ui;
  struct _Data data;
  struct _Config config;
};

struct _LgwResultsTextViewClassPrivate {
  GtkTextTagTable *text_tag_table;
};

#define LGW_RESULTSTEXTVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_RESULTSTEXTVIEW, LgwResultsTextViewPrivate));

G_END_DECLS

#endif

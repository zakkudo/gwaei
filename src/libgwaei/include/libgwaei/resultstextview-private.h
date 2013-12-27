#ifndef LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED
#define LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_ACTIONS,
    TOTAL_PROPS
} Props;

typedef enum {
  SIGNALID_FOCUS_IN_EVENT,
  SIGNALID_FOCUS_OUT_EVENT,
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
  GList *action_group_list;
  LgwActionGroup *action_group;
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

#include <libgwaei/resultstextview-callbacks.h>

static GList* lgw_resultstextview_get_actions (LgwActionable *actionable);
static void lgw_resultstextview_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);

void lgw_resultstextview_sync_actions (LgwResultsTextView *results_text_view);

#endif

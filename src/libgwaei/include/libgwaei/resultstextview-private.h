#ifndef LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED
#define LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_ACTIONS,
    PROP_TEXTTAGTABLE,
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
  LgwTextTagTable *text_tag_table;
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
  gint unused;
};

#define LGW_RESULTSTEXTVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_RESULTSTEXTVIEW, LgwResultsTextViewPrivate));

G_END_DECLS

#include <libgwaei/resultstextview-callbacks.h>
#include <libgwaei/resultstextview-actionable-interface.h>
#include <libgwaei/resultstextview-resultsview-interface.h>

#endif

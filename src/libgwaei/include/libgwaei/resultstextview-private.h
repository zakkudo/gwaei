#ifndef LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED
#define LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  TOTAL_LGW_RESULTSTEXTVIEW_CLASS_SIGNALIDS
} LgwResultsTextViewClassSignalId;

struct _UI {
    GtkBox *box;
    GtkTextView *text_view;
    GtkTextBuffer *text_buffer;
};

struct _LgwResultsTextViewPrivate {
    struct _UI ui;
};

struct _LgwResultsTextViewClassPrivate {
  GtkTextTagTable *text_tag_table;
  guint signalid[TOTAL_LGW_RESULTSTEXTVIEW_CLASS_SIGNALIDS];
};

#define LGW_RESULTSTEXTVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_RESULTSTEXTVIEW, LgwResultsTextViewPrivate));

G_END_DECLS

#endif

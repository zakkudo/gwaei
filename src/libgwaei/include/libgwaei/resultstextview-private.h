#ifndef LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED
#define LGW_RESULTSTEXTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
    GtkBox *box;
    GtkTextView *text_view;
    GtkTextBuffer *text_buffer;
};

struct _LgwResultsTextViewPrivate {
    struct _UI ui;
};

#define LGW_RESULTSTEXTVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_RESULTSTEXTVIEW, LgwResultsTextViewPrivate));

G_END_DECLS

#endif

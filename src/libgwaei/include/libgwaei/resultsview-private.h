#ifndef LGW_RESULTSVIEW_PRIVATE_INCLUDED
#define LGW_RESULTSVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
    GtkBox *box;
    GtkNotebook *notebook;
};

struct _LgwResultsViewPrivate {
    struct _UI ui;
};

#define LGW_RESULTSVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_RESULTSVIEW, LgwResultsViewPrivate));

G_END_DECLS

#endif

#ifndef LGW_SEARCHWIDGET_PRIVATE_INCLUDED
#define LGW_SEARCHWIDGET_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
    GtkBox *box;
    GtkPaned *paned;
    LgwSearchEntry *search_entry;
    GtkSearchBar *search_bar;
    LgwResultsTextView *results_view;
    LgwDictionaryView *dictionary_view;
};

struct _LgwSearchWidgetPrivate {
    struct _UI ui;
};

#define LGW_SEARCHWIDGET_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_SEARCHWIDGET, LgwSearchWidgetPrivate));

G_END_DECLS

#endif

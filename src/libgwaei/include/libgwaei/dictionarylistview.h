#ifndef LGW_DICTIONARYLISTVIEW_INCLUDED
#define LGW_DICTIONARYLISTVIEW_INCLUDED

G_BEGIN_DECLS

typedef enum {
  TOTAL_LGW_DICTIONARYLISTVIEW_CLASS_SIGNALIDS
} LgwDictionaryListViewClassSignalId;


//Boilerplate
typedef struct _LgwDictionaryListView LgwDictionaryListView;
typedef struct _LgwDictionaryListViewClass LgwDictionaryListViewClass;
typedef struct _LgwDictionaryListViewPrivate LgwDictionaryListViewPrivate;

#define LGW_TYPE_DICTIONARYLISTVIEW              (lgw_dictionarylistview_get_type())
#define LGW_DICTIONARYLISTVIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_DICTIONARYLISTVIEW, LgwDictionaryListView))
#define LGW_DICTIONARYLISTVIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_DICTIONARYLISTVIEW, LgwDictionaryListViewClass))
#define LGW_IS_DICTIONARYLISTVIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_DICTIONARYLISTVIEW))
#define LGW_IS_DICTIONARYLISTVIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_DICTIONARYLISTVIEW))
#define LGW_DICTIONARYLISTVIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_DICTIONARYLISTVIEW, LgwDictionaryListViewClass))

struct _LgwDictionaryListView {
  GtkBox box;
  LgwDictionaryListViewPrivate *priv;
};

struct _LgwDictionaryListViewClass {
  GtkBoxClass parent_class;
  guint signalid[TOTAL_LGW_DICTIONARYLISTVIEW_CLASS_SIGNALIDS];
};

//Methods

GtkWidget* lgw_dictionarylistview_new (void);
GType lgw_dictionarylistview_get_type (void) G_GNUC_CONST;

void lgw_dictionarylistview_set_dictionarylist (LgwDictionaryListView *view, LgwDictionaryList *dictionary_list);
LgwDictionaryList* lgw_dictionarylistview_get_dictionarylist (LgwDictionaryListView *view);


G_END_DECLS

#endif

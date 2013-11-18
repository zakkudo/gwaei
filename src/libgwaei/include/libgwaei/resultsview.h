#ifndef LGW_RESULTSVIEW_INCLUDED
#define LGW_RESULTSVIEW_INCLUDED

G_BEGIN_DECLS

typedef enum {
  TOTAL_LGW_RESULTSVIEW_CLASS_SIGNALIDS
} LgwResultsViewClassSignalId;


//Boilerplate
typedef struct _LgwResultsView LgwResultsView;
typedef struct _LgwResultsViewClass LgwResultsViewClass;
typedef struct _LgwResultsViewPrivate LgwResultsViewPrivate;

#define LGW_TYPE_RESULTSVIEW              (lgw_resultsview_get_type())
#define LGW_RESULTSVIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_RESULTSVIEW, LgwResultsView))
#define LGW_RESULTSVIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_RESULTSVIEW, LgwResultsViewClass))
#define LGW_IS_RESULTSVIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_RESULTSVIEW))
#define LGW_IS_RESULTSVIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_RESULTSVIEW))
#define LGW_RESULTSVIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_RESULTSVIEW, LgwResultsViewClass))

struct _LgwResultsView {
  GtkBox box;
  LgwResultsViewPrivate *priv;
};

struct _LgwResultsViewClass {
  GtkBoxClass parent_class;
  guint signalid[TOTAL_LGW_RESULTSVIEW_CLASS_SIGNALIDS];
};

GtkWidget* lgw_resultsview_new (void);
GType lgw_resultsview_get_type (void) G_GNUC_CONST;


G_END_DECLS

#endif

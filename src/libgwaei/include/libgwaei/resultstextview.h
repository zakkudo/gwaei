#ifndef LGW_RESULTSTEXTVIEW_INCLUDED
#define LGW_RESULTSTEXTVIEW_INCLUDED

G_BEGIN_DECLS

typedef enum {
  TOTAL_LGW_RESULTSTEXTVIEW_CLASS_SIGNALIDS
} LgwResultsTextViewClassSignalId;


//Boilerplate
typedef struct _LgwResultsTextView LgwResultsTextView;
typedef struct _LgwResultsTextViewClass LgwResultsTextViewClass;
typedef struct _LgwResultsTextViewPrivate LgwResultsTextViewPrivate;

#define LGW_TYPE_RESULTSTEXTVIEW              (lgw_resultstextview_get_type())
#define LGW_RESULTSTEXTVIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_RESULTSTEXTVIEW, LgwResultsTextView))
#define LGW_RESULTSTEXTVIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_RESULTSTEXTVIEW, LgwResultsTextViewClass))
#define LGW_IS_RESULTSTEXTVIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_RESULTSTEXTVIEW))
#define LGW_IS_RESULTSTEXTVIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_RESULTSTEXTVIEW))
#define LGW_RESULTSTEXTVIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_RESULTSTEXTVIEW, LgwResultsTextViewClass))

struct _LgwResultsTextView {
  GtkBox box;
  LgwResultsTextViewPrivate *priv;
};

struct _LgwResultsTextViewClass {
  GtkBoxClass parent_class;
  GtkTextTagTable *text_tag_table;
  guint signalid[TOTAL_LGW_RESULTSTEXTVIEW_CLASS_SIGNALIDS];
};

GtkWidget* lgw_resultstextview_new (void);
GType lgw_resultstextview_get_type (void) G_GNUC_CONST;


G_END_DECLS

#endif

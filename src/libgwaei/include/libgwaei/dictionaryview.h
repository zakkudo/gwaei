#ifndef LGW_DICTIONARYVIEW_INCLUDED
#define LGW_DICTIONARYVIEW_INCLUDED

G_BEGIN_DECLS

typedef enum {
  TOTAL_LGW_DICTIONARYVIEW_CLASS_SIGNALIDS
} LgwDictionaryViewClassSignalId;


//Boilerplate
typedef struct _LgwDictionaryView LgwDictionaryView;
typedef struct _LgwDictionaryViewClass LgwDictionaryViewClass;
typedef struct _LgwDictionaryViewPrivate LgwDictionaryViewPrivate;

#define LGW_TYPE_DICTIONARYVIEW              (lgw_dictionaryview_get_type())
#define LGW_DICTIONARYVIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_DICTIONARYVIEW, LgwDictionaryView))
#define LGW_DICTIONARYVIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_DICTIONARYVIEW, LgwDictionaryViewClass))
#define LGW_IS_DICTIONARYVIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_DICTIONARYVIEW))
#define LGW_IS_DICTIONARYVIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_DICTIONARYVIEW))
#define LGW_DICTIONARYVIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_DICTIONARYVIEW, LgwDictionaryViewClass))

struct _LgwDictionaryView {
  GtkBox box;
  LgwDictionaryViewPrivate *priv;
};

struct _LgwDictionaryViewClass {
  GtkBoxClass parent_class;
  guint signalid[TOTAL_LGW_DICTIONARYVIEW_CLASS_SIGNALIDS];
};

//Methods

GtkWidget* lgw_dictionaryview_new (void);
GType lgw_dictionaryview_get_type (void) G_GNUC_CONST;

void lgw_dictionaryview_set_dictionarylist (LgwDictionaryView *view, LgwDictionaryList *dictionary_list);
LgwDictionaryList* lgw_dictionaryview_get_dictionarylist (LgwDictionaryView *view);


G_END_DECLS

#endif

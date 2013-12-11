#ifndef LGW_SEARCHWIDGET_INCLUDED
#define LGW_SEARCHWIDGET_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LgwSearchWidget LgwSearchWidget;
typedef struct _LgwSearchWidgetClass LgwSearchWidgetClass;
typedef struct _LgwSearchWidgetPrivate LgwSearchWidgetPrivate;

#define LGW_TYPE_SEARCHWIDGET              (lgw_searchwidget_get_type())
#define LGW_SEARCHWIDGET(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_SEARCHWIDGET, LgwSearchWidget))
#define LGW_SEARCHWIDGET_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_SEARCHWIDGET, LgwSearchWidgetClass))
#define LGW_IS_SEARCHWIDGET(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_SEARCHWIDGET))
#define LGW_IS_SEARCHWIDGET_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_SEARCHWIDGET))
#define LGW_SEARCHWIDGET_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_SEARCHWIDGET, LgwSearchWidgetClass))

struct _LgwSearchWidget {
  GtkBox box;
  LgwSearchWidgetPrivate *priv;
};

struct _LgwSearchWidgetClass {
  GtkBoxClass parent_class;
};

GtkWidget* lgw_searchwidget_new (void);
GType lgw_searchwidget_get_type (void) G_GNUC_CONST;

//Methods

gboolean lgw_searchwidget_get_search_mode (LgwSearchWidget *widget);
void lgw_searchwidget_set_search_mode (LgwSearchWidget *widget, gboolean search_mode);

void lgw_searchwidget_set_dictionarylist (LgwSearchWidget *search_widget, LwDictionaryList *dictionary_list);

GList* lgw_searchwidget_get_actions (LgwActionable *widget);
void lgw_searchwidget_set_actiongroup (LgwActionable *actionable, LgwActionGroup *action_group);


G_END_DECLS

#endif

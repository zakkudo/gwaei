#ifndef LGW_VOCABULARYWIDGET_INCLUDED
#define LGW_VOCABULARYWIDGET_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LgwVocabularyWidget LgwVocabularyWidget;
typedef struct _LgwVocabularyWidgetClass LgwVocabularyWidgetClass;
typedef struct _LgwVocabularyWidgetPrivate LgwVocabularyWidgetPrivate;
typedef struct _LgwVocabularyWidgetClassPrivate LgwVocabularyWidgetClassPrivate;

#define LGW_TYPE_VOCABULARYWIDGET              (lgw_vocabularywidget_get_type())
#define LGW_VOCABULARYWIDGET(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_VOCABULARYWIDGET, LgwVocabularyWidget))
#define LGW_VOCABULARYWIDGET_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_VOCABULARYWIDGET, LgwVocabularyWidgetClass))
#define LGW_IS_VOCABULARYWIDGET(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_VOCABULARYWIDGET))
#define LGW_IS_VOCABULARYWIDGET_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_VOCABULARYWIDGET))
#define LGW_VOCABULARYWIDGET_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_VOCABULARYWIDGET, LgwVocabularyWidgetClass))

struct _LgwVocabularyWidget {
  GtkBox box;
  LgwVocabularyWidgetPrivate *priv;
};

struct _LgwVocabularyWidgetClass {
  GtkBoxClass parent_class;
  LgwVocabularyWidgetClassPrivate *priv;
};

GtkWidget* lgw_vocabularywidget_new (void);
GType lgw_vocabularywidget_get_type (void) G_GNUC_CONST;

//Methods


void lgw_vocabularywidget_set_preferences (LgwVocabularyWidget *vocabulary_widget, LwPreferences *preferences);
LwPreferences* lgw_vocabularywidget_get_preferences (LgwVocabularyWidget *vocabulary_widget);

void lgw_vocabularywidget_set_liststore (LgwVocabularyWidget *vocabulary_widget, LgwVocabularyListStore *vocabulary_list_store);
LgwVocabularyListStore* lgw_vocabularywidget_get_liststore (LgwVocabularyWidget *vocabulary_widget);

G_END_DECLS

#endif

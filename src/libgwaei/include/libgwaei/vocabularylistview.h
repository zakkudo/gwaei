#ifndef LGW_VOCABULARYLISTVIEW_INCLUDED
#define LGW_VOCABULARYLISTVIEW_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LgwVocabularyListView LgwVocabularyListView;
typedef struct _LgwVocabularyListViewClass LgwVocabularyListViewClass;
typedef struct _LgwVocabularyListViewPrivate LgwVocabularyListViewPrivate;
typedef struct _LgwVocabularyListViewClassPrivate LgwVocabularyListViewClassPrivate;

#define LGW_TYPE_VOCABULARYLISTVIEW              (lgw_vocabularylistview_get_type())
#define LGW_VOCABULARYLISTVIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_VOCABULARYLISTVIEW, LgwVocabularyListView))
#define LGW_VOCABULARYLISTVIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_VOCABULARYLISTVIEW, LgwVocabularyListViewClass))
#define LGW_IS_VOCABULARYLISTVIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_VOCABULARYLISTVIEW))
#define LGW_IS_VOCABULARYLISTVIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_VOCABULARYLISTVIEW))
#define LGW_VOCABULARYLISTVIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_VOCABULARYLISTVIEW, LgwVocabularyListViewClass))

struct _LgwVocabularyListView {
  GtkBox box;
  LgwVocabularyListViewPrivate *priv;
};

struct _LgwVocabularyListViewClass {
  GtkBoxClass parent_class;
  LgwVocabularyListViewClassPrivate *priv;
};

//Methods

GtkWidget* lgw_vocabularylistview_new (void);
GType lgw_vocabularylistview_get_type (void) G_GNUC_CONST;

void lgw_vocabularylistview_set_liststore (LgwVocabularyListView *vocabulary_list_view, LgwVocabularyListStore *vocabulary_list_store);
LgwVocabularyListStore* lgw_vocabularylistview_get_liststore (LgwVocabularyListView *vocabulary_list_view);


G_END_DECLS

#endif

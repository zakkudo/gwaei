#ifndef LGW_VOCABULARYWORDVIEW_INCLUDED
#define LGW_VOCABULARYWORDVIEW_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LgwVocabularyWordView LgwVocabularyWordView;
typedef struct _LgwVocabularyWordViewClass LgwVocabularyWordViewClass;
typedef struct _LgwVocabularyWordViewPrivate LgwVocabularyWordViewPrivate;
typedef struct _LgwVocabularyWordViewClassPrivate LgwVocabularyWordViewClassPrivate;

#define LGW_TYPE_VOCABULARYWORDVIEW              (lgw_vocabularywordview_get_type())
#define LGW_VOCABULARYWORDVIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_VOCABULARYWORDVIEW, LgwVocabularyWordView))
#define LGW_VOCABULARYWORDVIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_VOCABULARYWORDVIEW, LgwVocabularyWordViewClass))
#define LGW_IS_VOCABULARYWORDVIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_VOCABULARYWORDVIEW))
#define LGW_IS_VOCABULARYWORDVIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_VOCABULARYWORDVIEW))
#define LGW_VOCABULARYWORDVIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_VOCABULARYWORDVIEW, LgwVocabularyWordViewClass))

struct _LgwVocabularyWordView {
  GtkBox box;
  LgwVocabularyWordViewPrivate *priv;
};

struct _LgwVocabularyWordViewClass {
  GtkBoxClass parent_class;
  LgwVocabularyWordViewClassPrivate *priv;
};

//Methods

GtkWidget* lgw_vocabularywordview_new (void);
GType lgw_vocabularywordview_get_type (void) G_GNUC_CONST;

void lgw_vocabularywordview_set_wordstores (LgwVocabularyWordView *self, GList *wordstores);
LgwVocabularyWordStore* lgw_vocabularywordview_get_wordstore (LgwVocabularyWordView *self);
GList* lgw_vocabularywordview_get_wordstores (LgwVocabularyWordView *self);

void lgw_vocabularywordview_delete_selected (LgwVocabularyWordView *self);
void lgw_vocabularywordview_add_new (LgwVocabularyWordView *self);
void lgw_vocabularywordview_save (LgwVocabularyWordView *self);
void lgw_vocabularywordview_revert (LgwVocabularyWordView *self);

GList* lgw_vocabularywordview_get_selected_words (LgwVocabularyWordView *self);
GList* lgw_treeview_get_selected_words (GtkTreeView *self);
void lgw_vocabularywordview_select_words (LgwVocabularyWordView *self, GList *words);

G_END_DECLS


#endif

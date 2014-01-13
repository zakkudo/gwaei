#ifndef LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED
#define LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_VOCABULARYWORDSTORE,
  TOTAL_PROPS
} Prop;

struct _UI {
  GtkScrolledWindow *scrolled_window;
  GtkTreeView *tree_view;
  GtkToolbar *toolbar;
  GtkBox *box;
};

struct _Data {
  LgwVocabularyWordStore *vocabulary_word_store;
};

struct _LgwVocabularyWordViewPrivate {
  struct _Data data;
  struct _UI ui;
};

struct _LgwVocabularyWordViewClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_VOCABULARYWORDVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYWORDVIEW, LgwVocabularyWordViewPrivate))

G_END_DECLS

#endif



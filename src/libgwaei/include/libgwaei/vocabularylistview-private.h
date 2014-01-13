#ifndef LGW_VOCABULARYLISTVIEW_PRIVATE_INCLUDED
#define LGW_VOCABULARYLISTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_VOCABULARYLISTSTORE,
  TOTAL_PROPS
} Prop;

struct _Data {
  LgwVocabularyListStore *vocabulary_list_store;
};

struct _UI {
  GtkBox *box;
  GtkScrolledWindow *scrolled_window;
  GtkTreeView *tree_view;
  GtkToolbar *toolbar;
};

struct _LgwVocabularyListViewPrivate {
  struct _Data data;
  struct _UI ui;
};

struct _LgwVocabularyListViewClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_VOCABULARYLISTVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYLISTVIEW, LgwVocabularyListViewPrivate))

G_END_DECLS

#endif



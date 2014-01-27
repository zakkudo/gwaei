#ifndef LGW_VOCABULARYLISTVIEW_PRIVATE_INCLUDED
#define LGW_VOCABULARYLISTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_VOCABULARYLISTSTORE,
  PROP_VOCABULARYWORDVIEW,
  TOTAL_PROPS
} Prop;

typedef enum {
  SIGNALID_SELECTION_CHANGED,
  TOTAL_SIGNALIDS
} Signalid;

struct _Data {
  LgwVocabularyListStore *vocabulary_list_store;
  GtkTreeSelection *tree_selection;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _UI {
  GtkBox *box;
  GtkScrolledWindow *scrolled_window;
  GtkTreeView *tree_view;
  GtkToolbar *toolbar;
};


struct _Config {
  LgwVocabularyWordView *vocabulary_word_view;
};

struct _LgwVocabularyListViewPrivate {
  struct _Data data;
  struct _UI ui;
  struct _Config config;
};

struct _LgwVocabularyListViewClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_VOCABULARYLISTVIEW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_VOCABULARYLISTVIEW, LgwVocabularyListViewPrivate))

G_END_DECLS

#include <libgwaei/vocabularylistview-callbacks.h>

#endif



#ifndef LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED
#define LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_ACTIONS,
  PROP_VOCABULARYWORDSTORE,
  TOTAL_PROPS
} Prop;

typedef enum {
  SIGNALID_SELECTION_CHANGED,
  TOTAL_SIGNALIDS
} Signalid;

struct _UI {
  GtkScrolledWindow *scrolled_window;
  GtkTreeView *tree_view;
  GtkToolbar *toolbar;
  GtkBox *box;
};

struct _Data {
  LgwVocabularyWordStore *vocabulary_word_store;
  GtkTreeSelection *tree_selection;
  LgwActionGroup *action_group;
  GList *action_group_list;
  guint signalid[TOTAL_SIGNALIDS];
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

#include <libgwaei/vocabularywordview-callbacks.h>
#include <libgwaei/vocabularywordview-actionable-interface.h>

#endif



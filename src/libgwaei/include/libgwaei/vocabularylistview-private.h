#ifndef LGW_VOCABULARYLISTVIEW_PRIVATE_INCLUDED
#define LGW_VOCABULARYLISTVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_ACTIONS,
  PROP_VOCABULARYLISTSTORE,
  PROP_VOCABULARYWORDVIEW,
  TOTAL_PROPS
} Prop;

typedef enum {
  SIGNALID_SELECTION_CHANGED,
  SIGNALID_FOCUS_IN_EVENT,
  SIGNALID_FOCUS_OUT_EVENT,
  SIGNALID_NAME_EDITED,
  SIGNALID_DRAG_MOTION,
  SIGNALID_DRAG_DROP,
  SIGNALID_DRAG_DATA_RECEIVED,
  SIGNALID_DRAG_LEAVE,
  TOTAL_SIGNALIDS
} Signalid;

typedef enum {
  TREEVIEWCOLUMN_POSITION,
  TREEVIEWCOLUMN_SAVED_POSITION,
  TREEVIEWCOLUMN_NAME,
  TOTAL_TREEVIEWCOLUMNS
} TreeViewColumn;

typedef enum {
  CELLRENDERER_POSITION,
  CELLRENDERER_SAVED_POSITION,
  CELLRENDERER_NAME,
  TOTAL_CELLRENDERERS
} CellRenderer;

struct _Data {
  LgwVocabularyListStore *vocabulary_list_store;
  GtkTreeSelection *tree_selection;
  LgwActionGroup *action_group;
  GList *action_group_list;
  guint signalid[TOTAL_SIGNALIDS];
  GdkDragAction suggested_action;
};

struct _UI {
  GtkBox *box;
  GtkScrolledWindow *scrolled_window;
  GtkTreeView *tree_view;
  GtkToolbar *toolbar;
  GtkTreeViewColumn *tree_view_column[TOTAL_TREEVIEWCOLUMNS];
  GtkCellRenderer *cell_renderer[TOTAL_CELLRENDERERS];
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
#include <libgwaei/vocabularylistview-actionable-interface.h>

#endif



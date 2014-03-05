#ifndef LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED
#define LGW_VOCABULARYWORDVIEW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_ACTIONS,
  PROP_VOCABULARYWORDSTORES,
  TOTAL_PROPS
} Prop;

typedef enum {
  SIGNALID_SELECTION_CHANGED,
  SIGNALID_FOCUS_IN_EVENT,
  SIGNALID_FOCUS_OUT_EVENT,
  SIGNALID_DRAG_MOTION,
  SIGNALID_DRAG_DROP,
  SIGNALID_DRAG_DATA_RECEIVED,
  SIGNALID_DRAG_LEAVE,
  TOTAL_SIGNALIDS
} Signalid;

typedef enum {
  TREEVIEWCOLUMN_POSITION,
  TREEVIEWCOLUMN_SAVED_POSITION,
  TREEVIEWCOLUMN_KANJI,
  TREEVIEWCOLUMN_READING,
  TREEVIEWCOLUMN_MEANING,
  TREEVIEWCOLUMN_DEFINITION,
  TOTAL_TREEVIEWCOLUMNS
} TreeViewColumn;

typedef enum {
  CELLRENDERER_POSITION,
  CELLRENDERER_SAVED_POSITION,
  CELLRENDERER_KANJI,
  CELLRENDERER_READING,
  CELLRENDERER_MEANING,
  CELLRENDERER_DEFINITION,
  TOTAL_CELLRENDERERS
} CellRenderer;

struct _UI {
  GtkScrolledWindow *scrolled_window;
  GtkTreeView *tree_view;
  GtkToolbar *toolbar;
  GtkBox *box;
  GtkTreeViewColumn *tree_view_column[TOTAL_TREEVIEWCOLUMNS];
  GtkCellRenderer *cell_renderer[TOTAL_CELLRENDERERS];
};

struct _Data {
  GList *vocabulary_word_stores;
  LgwVocabularyWordStore *vocabulary_word_store;
  GtkTreeSelection *tree_selection;
  LgwActionGroup *action_group;
  GList *action_group_list;
  guint signalid[TOTAL_SIGNALIDS];
  gboolean editable;
  GdkDragAction suggested_action;
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



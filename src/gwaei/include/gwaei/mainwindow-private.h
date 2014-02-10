#ifndef GW_MAINWINDOW_PRIVATE_INCLUDED
#define GW_MAINWINDOW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_ACTIONS,
    TOTAL_PROPS
} Props;

typedef enum {
  SIGNALID_APPLICATION_PROPERTY_CHANGED,
  SIGNALID_STACK_VISIBLE_CHILD_PROPERTY_CHANGED,
  SIGNALID_SHOW_MENUBAR,
  SIGNALID_SEARCHWIDGET_ACTIONS,
  SIGNALID_VOCABULARYWIDGET_ACTIONS,
  TOTAL_SIGNALIDS
} SignalId;

typedef enum {
  CLASS_SIGNALID_WORD_ADDED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _UI {
    GtkStackSwitcher *stack_switcher;
    GtkHeaderBar *header_bar;
    GtkStack *stack;

    LgwSearchWidget *search_widget;
    LgwVocabularyWidget *vocabulary_widget;
};

struct _Data {
  guint signalid[TOTAL_SIGNALIDS];
  gint mouse_button_press_x;
  gint mouse_button_press_y;
  gint mouse_button_press_root_x;
  gint mouse_button_press_root_y;
  gunichar mouse_button_character;
  gchar* mouse_hovered_word; 
  LgwActionGroup *action_group;
  GList *action_group_list;
};

struct _GwMainWindowPrivate {
  struct _UI ui;
  struct _Data data;
};

struct _GwMainWindowClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define GW_MAINWINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GW_TYPE_MAINWINDOW, GwMainWindowPrivate))

G_END_DECLS

#include <gwaei/mainwindow-callbacks.h>
#include <gwaei/mainwindow-actionable-interface.h>

#endif

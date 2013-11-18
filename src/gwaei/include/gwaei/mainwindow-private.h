#ifndef GW_MAINWINDOW_PRIVATE_INCLUDED
#define GW_MAINWINDOW_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
    GtkBox *box;

    GtkStackSwitcher *stack_switcher;
    GtkMenuButton *menu_button;
    GtkHeaderBar *header_bar;
    GtkStack *stack;

    //to be removed
    LgwSearchWidget *search_widget;
    GtkSearchBar *vocabulary_widget;
};

struct _GwMainWindowPrivate {
  struct _UI ui;

  //Mouse variables
  gint mouse_button_press_x;
  gint mouse_button_press_y;
  gint mouse_button_press_root_x;
  gint mouse_button_press_root_y;
  gunichar mouse_button_character;
  gchar* mouse_hovered_word; 
};

#define GW_MAINWINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GW_TYPE_MAINWINDOW, GwMainWindowPrivate))

G_END_DECLS

#endif

#ifndef LGW_WINDOW_INCLUDED
#define LGW_WINDOW_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LgwWindow LgwWindow;
typedef struct _LgwWindowClass LgwWindowClass;
typedef struct _LgwWindowPrivate LgwWindowPrivate;
typedef struct _LgwWindowClassPrivate LgwWindowClassPrivate;

#define LGW_TYPE_WINDOW              (lgw_window_get_type())
#define LGW_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_WINDOW, LgwWindow))
#define LGW_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_WINDOW, LgwWindowClass))
#define LGW_IS_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_WINDOW))
#define LGW_IS_WINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_WINDOW))
#define LGW_WINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_WINDOW, LgwWindowClass))

struct _LgwWindow {
  GtkApplicationWindow window;
  LgwWindowPrivate *priv;
};

struct _LgwWindowClass {
  GtkApplicationWindowClass parent_class;
  LgwWindowClassPrivate *priv;
};

//Methods
GtkWindow* lgw_window_new (void);
GType lgw_window_get_type (void) G_GNUC_CONST;

GMenuModel* lgw_window_get_window_menumodel (LgwWindow *window);
GMenuModel* lgw_window_get_button_menumodel (LgwWindow *window);

void lgw_window_set_window_menumodel (LgwWindow *window, GMenuModel *menu_model);
void lgw_window_set_button_menumodel (LgwWindow *window, GMenuModel *menu_model);

gboolean lgw_window_is_closable (LgwWindow *window);

GMenuModel* lgw_window_get_transient_for_menumodel (LgwWindow *window);

void  lgw_window_show_menubar (LgwWindow *window, gboolean show);

GtkWidget* lgw_window_get_menubar (LgwWindow *window);
GtkWidget* lgw_window_get_menu_button (LgwWindow *window);

void lgw_window_set_shows_menubar (LgwWindow *window, gboolean can_show_menubar);
gboolean lgw_window_get_shows_menubar (LgwWindow *window);

void lgw_window_pack_start (LgwWindow *window, GtkWidget *widget, gboolean expand, gboolean fill, guint padding);

void lgw_window_clear_actions (LgwWindow *window);
void lgw_window_add_actions (LgwWindow *window, GList *action_group_list);
void lgw_window_set_actions (LgwWindow *window, GList *action_group_list);

G_END_DECLS

#endif

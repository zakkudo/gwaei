#ifndef LGW_WINDOW_PRIVATE_INCLUDED
#define LGW_WINDOW_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_WINDOW_MENUMODEL,
    PROP_BUTTON_MENUMODEL,
    PROP_WINDOW_MENUBAR,
    PROP_SHOWS_MENUBAR,
    PROP_CLOSABLE,
    TOTAL_PROPS
} Prop;

typedef enum {
  SIGNALID_CONFIGURE_EVENT,
  SIGNALID_FOCUS_IN_EVENT,
  SIGNALID_SHOW_MENUBAR,
  SIGNALID_APPLICATION_PROPERTY_CHANGED,
  TOTAL_SIGNALIDS
} SignalId;

struct _UI {
  GtkMenuBar *window_menubar;
  GtkMenuButton *menu_button;
  GtkBox *box;
};

struct _Config {
  gboolean shows_menubar;
  gboolean show_menubar;
  gboolean is_closable;
};

struct _Data {
  GMenuModel *window_menu_model;
  GMenuModel *button_menu_model;
  gboolean important;
  GtkAccelGroup *accelgroup;
  gint x, y, width, height;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _LgwWindowPrivate {
  struct _UI ui;
  struct _Data data;
  struct _Config config;
};

struct _LgwWindowClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LGW_WINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_WINDOW, LgwWindowPrivate))

void lgw_window_sync_menubar_show (LgwWindow *window);

G_END_DECLS

#include <libgwaei/window-callbacks.h>

#endif


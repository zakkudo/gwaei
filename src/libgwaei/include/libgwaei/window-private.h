#ifndef LGW_WINDOW_PRIVATE_INCLUDED
#define LGW_WINDOW_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _UI {
  GtkMenuBar *menubar;
};

struct _Data {
  GMenuModel *menu_model;
  gboolean important;
  GtkAccelGroup *accelgroup;
  gint x, y, width, height;
};

struct _LgwWindowPrivate {
  struct _UI ui;
  struct _Data data;
};

#define LGW_WINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LGW_TYPE_WINDOW, LgwWindowPrivate))

G_END_DECLS

#endif


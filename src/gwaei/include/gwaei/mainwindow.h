#ifndef GW_MAINWINDOW_INCLUDED
#define GW_MAINWINDOW_INCLUDED

#include <libwaei/search.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _GwMainWindow GwMainWindow;
typedef struct _GwMainWindowClass GwMainWindowClass;
typedef struct _GwMainWindowPrivate GwMainWindowPrivate;
typedef struct _GwMainWindowClassPrivate GwMainWindowClassPrivate;

#define GW_TYPE_MAINWINDOW              (gw_mainwindow_get_type())
#define GW_MAINWINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GW_TYPE_MAINWINDOW, GwMainWindow))
#define GW_MAINWINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GW_TYPE_MAINWINDOW, GwMainWindowClass))
#define GW_IS_MAINWINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GW_TYPE_MAINWINDOW))
#define GW_IS_MAINWINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GW_TYPE_MAINWINDOW))
#define GW_MAINWINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GW_TYPE_MAINWINDOW, GwMainWindowClass))

struct _GwMainWindow {
  GtkWindow window;
  GwMainWindowPrivate *priv;
};

struct _GwMainWindowClass {
  GtkWindowClass parent_class;
  GwMainWindowClassPrivate *priv;
};

GtkWindow* gw_mainwindow_new (GtkApplication *application);
GType gw_mainwindow_get_type (void) G_GNUC_CONST;


GList* gw_mainwindow_get_actions (GwMainWindow *window);

G_END_DECLS

#endif

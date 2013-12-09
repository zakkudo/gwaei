#ifndef GW_MAINWINDOW_CALLBACKS_INCLUDED
#define GW_MAINWINDOW_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void gw_mainwindow_connect_signals (GwMainWindow *window);
void gw_mainwindow_disconnect_signals (GwMainWindow *window);

void gw_mainwindow_application_property_changed_cb (GwMainWindow *main_window, GParamSpec *pspec, gpointer data);
void gw_mainwindow_application_visible_child_property_changed_cb (GwMainWindow *main_window, GParamSpec *pspec, gpointer data);

void gw_mainwindow_close_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

G_END_DECLS

#endif

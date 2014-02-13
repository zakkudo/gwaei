#ifndef LGW_WINDOW_CALLBACKS_INCLUDED
#define LGW_WINDOW_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_window_connect_signals (LgwWindow *self);
void lgw_window_disconnect_signals (LgwWindow *self);

gboolean lgw_window_configure_event_cb (LgwWindow *self, GdkEvent *event, gpointer data);
gboolean lgw_window_focus_in_event_cb (LgwWindow *self, GdkEvent *event, gpointer data);

void lgw_window_sync_show_menubar_cb (GSettings *settings, gchar *key, gpointer data);

void lgw_window_application_property_changed_cb (LgwWindow *self, GParamSpec *pspec, gpointer data);

void lgw_window_set_focus_cb (LgwWindow *self, GtkWidget *widget, gpointer data);

G_END_DECLS

#endif

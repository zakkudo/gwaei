#ifndef LGW_WINDOW_CALLBACKS_INCLUDED
#define LGW_WINDOW_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_window_connect_signals (LgwWindow *window);
void lgw_window_disconnect_signals (LgwWindow *window);

gboolean lgw_window_configure_event_cb (GtkWidget *widget, GdkEvent *event, gpointer data);
gboolean lgw_window_focus_in_event_cb (GtkWidget *widget, GdkEvent *event, gpointer data);

void lgw_window_sync_show_menubar_cb (GSettings *settings, gchar *key, gpointer data);

void lgw_window_application_property_changed_cb (LgwWindow *window, GParamSpec *pspec, gpointer data);

void lgw_window_set_focus_cb (LgwWindow *window, GtkWidget *widget, gpointer data);

G_END_DECLS

#endif

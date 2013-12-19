#ifndef LGW_WINDOW_CALLBACKS_INCLUDED
#define LGW_WINDOW_CALLBACKS_INCLUDED

G_BEGIN_DECLS

gboolean lgw_window_configure_event_cb (GtkWidget *widget, GdkEvent *event, gpointer data);
gboolean lgw_window_focus_in_event_cb (GtkWidget *widget, GdkEvent *event, gpointer data);

G_END_DECLS

#endif

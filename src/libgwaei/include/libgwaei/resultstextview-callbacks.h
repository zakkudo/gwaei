#ifndef LGW_RESULTSTEXTVIEW_CALLBACKS_INCLUDED
#define LGW_RESULTSTEXTVIEW_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_resultstextview_connect_signals (LgwResultsTextView *results_text_view);
void lgw_resultstextview_disconnect_signals (LgwResultsTextView *results_text_view);

gboolean lgw_resultstextview_focus_in_event_cb (LgwResultsTextView *results_text_view, GdkEvent *event, GtkTextView *internal_text_view);
gboolean lgw_resultstextview_focus_out_event_cb (LgwResultsTextView *results_text_view, GdkEvent *event, GtkTextView *internal_text_view);

void lgw_resultstextview_copy_cb (GSimpleAction *action, GVariant *parameter, gpointer data);

G_END_DECLS

#endif


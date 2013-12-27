#ifndef LGW_SEARCHWIDGET_CALLBACKS_INCLUDED
#define LGW_SEARCHWIDGET_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_searchwidget_connect_signals (LgwSearchWidget *search_widget);
void lgw_searchwidget_disconnect_signals (LgwSearchWidget *search_widget);

void lgw_searchwidget_child_actions_property_changed_cb (LgwSearchWidget *search_widget, GParamSpec *pspec, LgwActionable *actionable);

G_END_DECLS

#endif

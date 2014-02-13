#ifndef LGW_SEARCHWIDGET_CALLBACKS_INCLUDED
#define LGW_SEARCHWIDGET_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_searchwidget_connect_signals (LgwSearchWidget *self);
void lgw_searchwidget_disconnect_signals (LgwSearchWidget *self);

void lgw_searchwidget_child_actions_property_changed_cb (LgwSearchWidget *self, GParamSpec *pspec, LgwActionable *actionable);

G_END_DECLS

#endif

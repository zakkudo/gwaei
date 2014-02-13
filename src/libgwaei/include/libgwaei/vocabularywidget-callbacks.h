#ifndef LGW_VOCABULARYWIDGET_CALLBACKS_INCLUDED
#define LGW_VOCABULARYWIDGET_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void lgw_vocabularywidget_connect_signals (LgwVocabularyWidget *self);
void lgw_vocabularywidget_disconnect_signals (LgwVocabularyWidget *self);

void lgw_vocabularywidget_child_actions_property_changed_cb (LgwVocabularyWidget *self, GParamSpec *pspec, LgwActionable *actionable);

G_END_DECLS

#endif

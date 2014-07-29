#ifndef W_COMMAND_CALLBACKS_INCLUDED
#define W_COMMAND_CALLBACKS_INCLUDED

G_BEGIN_DECLS

void w_command_progress_primary_message_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);
void w_command_progress_secondary_message_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);
void w_command_progress_completed_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);
void w_command_progress_fraction_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);

G_END_DECLS

#endif

#ifndef W_COMMAND_CALLBACKS_INCLUDED
#define W_COMMAND_CALLBACKS_INCLUDED

gboolean w_command_append_result_timeout (gpointer);
void w_command_progress_cb (LwDictionary *dictionary, LwProgress *progress, WApplication *application);

#endif

#ifndef W_CONSOLE_CALLBACKS_INCLUDED
#define W_CONSOLE_CALLBACKS_INCLUDED

gboolean w_console_append_result_timeout (gpointer);
void w_console_progress_cb (LwDictionary *dictionary, LwProgress *progress, WApplication *application);

#endif

#ifndef GW_INSTALL_PROGRESS_CALLBACKS_INCLUDED
#define GW_INSTALL_PROGRESS_CALLBACKS_INCLUDED

gboolean gw_installprogresswindow_update_ui_timeout (gpointer);
void gw_installprogresswindow_start_cb (GtkWidget*, gpointer);
void gw_installprogresswindow_update_progress_cb (LwDictionary *dictionary, LwProgress *progress, GwInstallProgressWindow *window);

#endif

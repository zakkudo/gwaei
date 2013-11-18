#ifndef GW_INSTALLPROGRESSWINDOW_PRIVATE_INCLUDED
#define GW_INSTALLPROGRESSWINDOW_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _GwInstallProgressWindowPrivate {
  LwProgress *progress;

  GtkLabel *sequence_label;
  GtkLabel *action_label;
  GtkLabel *direct_object_label;

  GtkProgressBar* progressbar;
  GtkButton *cancel_button;
};

#define GW_INSTALLPROGRESSWINDOW_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GW_TYPE_INSTALLPROGRESSWINDOW, GwInstallProgressWindowPrivate))

G_END_DECLS

#endif

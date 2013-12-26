#ifndef GW_APPLICATION_CALLBACKS_INCLUDED
#define GW_APPLICATION_CALLBACKS_INCLUDED

G_BEGIN_DECLS

#ifdef HAVE_HUNSPELL
void gw_application_open_aboutdialog_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void gw_application_spellcheck_toggled_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void gw_application_open_help_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void gw_application_open_glossary_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
void gw_application_quit_cb (GSimpleAction *action, GVariant *parameter, gpointer data);
#endif

G_END_DECLS

#endif

#ifndef LW_COMMAND_INCLUDED
#define LW_COMMAND_INCLUDED

#include <gio/gio.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwCommand LwCommand;
typedef struct _LwCommandClass LwCommandClass;
typedef struct _LwCommandPrivate LwCommandPrivate;
typedef struct _LwCommandClassPrivate LwCommandClassPrivate;

typedef int(*LwCommandRunFunc) (LwCommand * self, gchar *subcommand_name, gpointer data);

#define LW_TYPE_COMMAND              (lw_command_get_type())
#define LW_COMMAND(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_COMMAND, LwCommand))
#define LW_COMMAND_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_COMMAND, LwCommandClass))
#define LW_IS_COMMAND(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_COMMAND))
#define LW_IS_COMMAND_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_COMMAND))
#define LW_COMMAND_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_COMMAND, LwCommandClass))

struct _LwCommand {
  GObject object;
  LwCommandPrivate *priv;
};

struct _LwCommandClass {
  GObjectClass parent_class;
  LwCommandClassPrivate *priv;

  //Virtual methods
  LwCommandRunFunc run;
};

//Methods

LwCommand* lw_command_new (GApplication *application, GApplicationCommandLine *command_line);
GType lw_command_get_type (void) G_GNUC_CONST;

void lw_command_print (LwCommand *self, const gchar *format, ...);
void lw_command_printerr (LwCommand *self, const gchar *format, ...);

gchar const* lw_command_get_locale (void);
gboolean lw_command_is_japanese_locale (void);

//Properties
GApplication* lw_command_get_application (LwCommand *self);
GApplicationCommandLine* lw_command_get_command_line (LwCommand *self);

void lw_command_set_parameter_string (LwCommand * self, gchar const * parameter_string);
gchar const * lw_command_get_parameter_string (LwCommand * self);

void lw_command_set_description (LwCommand * self, gchar const * DESCRIPTION);
gchar const * lw_command_get_description (LwCommand * self);

void lw_command_set_summary (LwCommand * self, gchar const * summary);
gchar const * lw_command_get_summary (LwCommand * self);

void lw_command_add_subcommand (LwCommand * self, LwCommand * subcommand);
LwCommand* lw_command_lookup_subcommand (LwCommand * self, gchar const * SUBCOMMAND_NAME);

GOptionEntry const * lw_command_get_option_entries (LwCommand *self);
void lw_command_set_option_entries (LwCommand * self, GOptionEntry const * option_entries);

void lw_command_run (LwCommand * self);

#endif

#ifndef LW_COMMAND_INCLUDED
#define LW_COMMAND_INCLUDED

#include <gio/gio.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwCommand LwCommand;
typedef struct _LwCommandClass LwCommandClass;
typedef struct _LwCommandPrivate LwCommandPrivate;
typedef struct _LwCommandClassPrivate LwCommandClassPrivate;

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
};

//Methods

LwCommand* lw_command_new (GApplication *application, GApplicationCommandLine *command_line);
GType lw_command_get_type (void) G_GNUC_CONST;

//Properties
GApplication* lw_command_get_application (LwCommand *self);
GApplicationCommandLine* lw_command_get_command_line (LwCommand *self);

void lw_command_print (LwCommand *self, const gchar *format, ...);
void lw_command_printerr (LwCommand *self, const gchar *format, ...);

#endif

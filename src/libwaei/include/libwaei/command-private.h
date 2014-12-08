#ifndef LW_COMMAND_PRIVATE_INCLUDED
#define LW_COMMAND_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_APPLICATION,
  PROP_APPLICATION_COMMAND_LINE,
  TOTAL_PROPS
} Props;

struct _LwCommandPrivate {
  gint* argc;
  gchar*** argv;
  GOptionContext *context;
  GApplication *application;
  GApplicationCommandLine *command_line;
};

struct _LwCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_COMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_COMMAND, LwCommandPrivate))

//Properties
static void lw_command_set_application (LwCommand *self, GApplication *application);
static void lw_command_set_command_line (LwCommand *self, GApplicationCommandLine *command_line);


G_END_DECLS

#endif

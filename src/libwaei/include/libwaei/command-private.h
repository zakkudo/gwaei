#ifndef LW_COMMAND_PRIVATE_INCLUDED
#define LW_COMMAND_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_NAME,
  PROP_APPLICATION,
  PROP_APPLICATION_COMMAND_LINE,
  PROP_PARAMETER_STRING,
  PROP_DESCRIPTION,
  PROP_SUMMARY,
  PROP_OPTION_ENTRIES,
  TOTAL_PROPS
} Prop;

typedef enum {
  TOTAL_SIGNALIDS
} SignalId;

typedef enum {
  CLASS_SIGNALID_RUN,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwCommandPrivate {
  gint argc;
  gchar * * argv;
  GApplication *application;
  GApplicationCommandLine *command_line;
  gchar *parameter_string;
  GOptionEntry * option_entries;
  guint signalid[TOTAL_SIGNALIDS];

  gchar * name;
  gchar * description;
  gchar * summary;

  gchar * command_name;
  GTree * subcommands;
  LwCommand * default_subcommand;
};

struct _LwCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_COMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_COMMAND, LwCommandPrivate))

//Properties
static void lw_command_set_application (LwCommand *self, GApplication *application);
static void lw_command_set_command_line (LwCommand *self, GApplicationCommandLine *command_line);

static void lw_command_set_name (LwCommand * self, gchar const  * NAME);
gchar const * lw_command_get_name (LwCommand * self);

G_END_DECLS

#endif

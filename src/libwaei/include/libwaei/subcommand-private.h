#ifndef LW_SUBCOMMAND_PRIVATE_INCLUDED
#define LW_SUBCOMMAND_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_NAME,
  PROP_OPTION_ENTRIES,
  TOTAL_PROPS
} LwSubCommandProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwSubCommandPrivate {
  gchar *name;
  gchar *descrioption;
  gchar *help_description;
  GOptionEntry * option_entries;
};

struct _LwSubCommandClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_SUBCOMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_SUBCOMMAND, LwSubCommandPrivate));

//Properties
static void lw_subcommand_set_name (LwSubCommand * self, gchar const  * NAME);

//Methods



G_END_DECLS

#endif


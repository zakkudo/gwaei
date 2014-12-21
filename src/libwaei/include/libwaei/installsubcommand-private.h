#ifndef LW_INSTALLSUBCOMMAND_PRIVATE_INCLUDED
#define LW_INSTALLSUBCOMMAND_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  TOTAL_PROPS
} LwInstallSubCommandProps;

struct _LwInstallSubCommandPrivate {
   gchar **dictionaryinstall_names;
   GList *dictionaryinstalls;
   gboolean list_switch;
};

struct _LwInstallSubCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_INSTALLSUBCOMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_INSTALLSUBCOMMAND, LwInstallSubCommandPrivate));

//Properties

//Methods

static void lw_installsubcommand_run (LwSubCommand *self, gchar *** argv, gint * argc);

G_END_DECLS

#endif


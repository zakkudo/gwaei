#ifndef LW_UNINSTALLSUBCOMMAND_PRIVATE_INCLUDED
#define LW_UNINSTALLSUBCOMMAND_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  TOTAL_PROPS
} Prop;

struct _LwUninstallSubCommandPrivate {
  gchar **dictionary_names;
  GList *dictionaries;
  gboolean list_switch;
};

struct _LwUninstallSubCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

//Properties

//Methods

static void lw_uninstallsubcommand_run (LwSubCommand *self, gchar *** argv, gint * argc);

G_END_DECLS

#endif


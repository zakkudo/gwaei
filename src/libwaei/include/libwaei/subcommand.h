#ifndef LW_SUBCOMMAND_INCLUDED
#define LW_SUBCOMMAND_INCLUDED

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwSubCommand LwSubCommand;
typedef struct _LwSubCommandClass LwSubCommandClass;
typedef struct _LwSubCommandPrivate LwSubCommandPrivate;
typedef struct _LwSubCommandClassPrivate LwSubCommandClassPrivate;

#define LW_TYPE_SUBCOMMAND              (lw_subcommand_get_type())
#define LW_SUBCOMMAND(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_SUBCOMMAND, LwSubCommand))
#define LW_SUBCOMMAND_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_SUBCOMMAND, LwSubCommandClass))
#define LW_IS_SUBCOMMAND(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_SUBCOMMAND))
#define LW_IS_SUBCOMMAND_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_SUBCOMMAND))
#define LW_SUBCOMMAND_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_SUBCOMMAND, LwSubCommandClass))

typedef void(*LwSubCommandRunFunc)(LwSubCommand* self, gchar *** argv, gint * argc);

struct _LwSubCommand {
  GObject object;
  LwSubCommandPrivate *priv;
};

struct _LwSubCommandClass {
  GObjectClass parent_class;
  LwSubCommandClassPrivate *priv;

  // Virtual methods
  LwSubCommandRunFunc run;
};

//Methods
GType lw_subcommand_get_type (void) G_GNUC_CONST;
void lw_subcommand_run (LwSubCommand *self, gchar *** argv, gint * argc);

GOptionGroup * lw_subcommand_build_option_group (LwSubCommand * self);

GOptionEntry const * lw_subcommand_get_option_entries (LwSubCommand * self);
void lw_subcommand_set_option_entries (LwSubCommand * self, GOptionEntry const * option_entries);

gchar const * lw_subcommand_get_name (LwSubCommand * self);

//Properties


G_END_DECLS

#endif

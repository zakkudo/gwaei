#ifndef LW_UNINSTALLSUBCOMMAND_INCLUDED
#define LW_UNINSTALLSUBCOMMAND_INCLUDED

#include "subcommand.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwUninstallSubCommand LwUninstallSubCommand;
typedef struct _LwUninstallSubCommandClass LwUninstallSubCommandClass;
typedef struct _LwUninstallSubCommandPrivate LwUninstallSubCommandPrivate;
typedef struct _LwUninstallSubCommandClassPrivate LwUninstallSubCommandClassPrivate;

#define LW_TYPE_UNINSTALLSUBCOMMAND              (lw_uninstallsubcommand_get_type())
#define LW_UNINSTALLSUBCOMMAND(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_UNINSTALLSUBCOMMAND, LwUninstallSubCommand))
#define LW_UNINSTALLSUBCOMMAND_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_UNINSTALLSUBCOMMAND, LwUninstallSubCommandClass))
#define LW_IS_UNINSTALLSUBCOMMAND(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_UNINSTALLSUBCOMMAND))
#define LW_IS_UNINSTALLSUBCOMMAND_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_UNINSTALLSUBCOMMAND))
#define LW_UNINSTALLSUBCOMMAND_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_UNINSTALLSUBCOMMAND, LwUninstallSubCommandClass))

struct _LwUninstallSubCommand {
  LwSubCommand object;
};

struct _LwUninstallSubCommandClass {
  LwSubCommandClass parent_class;
  LwUninstallSubCommandClassPrivate *priv;
};

//Methods
LwSubCommand* lw_uninstallsubcommand_new (void);
GType lw_uninstallsubcommand_get_type (void) G_GNUC_CONST;

//Properties


G_END_DECLS

#endif

#ifndef LW_INSTALLSUBCOMMAND_INCLUDED
#define LW_INSTALLSUBCOMMAND_INCLUDED

#include "subcommand.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwInstallSubCommand LwInstallSubCommand;
typedef struct _LwInstallSubCommandClass LwInstallSubCommandClass;
typedef struct _LwInstallSubCommandPrivate LwInstallSubCommandPrivate;
typedef struct _LwInstallSubCommandClassPrivate LwInstallSubCommandClassPrivate;

#define LW_TYPE_INSTALLSUBCOMMAND              (lw_installsubcommand_get_type())
#define LW_INSTALLSUBCOMMAND(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_INSTALLSUBCOMMAND, LwInstallSubCommand))
#define LW_INSTALLSUBCOMMAND_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_INSTALLSUBCOMMAND, LwInstallSubCommandClass))
#define LW_IS_INSTALLSUBCOMMAND(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_INSTALLSUBCOMMAND))
#define LW_IS_INSTALLSUBCOMMAND_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_INSTALLSUBCOMMAND))
#define LW_INSTALLSUBCOMMAND_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_INSTALLSUBCOMMAND, LwInstallSubCommandClass))

struct _LwInstallSubCommand {
  LwSubCommand object;
};

struct _LwInstallSubCommandClass {
  LwSubCommandClass parent_class;
  LwInstallSubCommandClassPrivate *priv;
};

//Methods
LwSubCommand* lw_installsubcommand_new (void);
GType lw_installsubcommand_get_type (void) G_GNUC_CONST;

//Properties


G_END_DECLS

#endif

#ifndef LW_SEARCHSUBCOMMAND_INCLUDED
#define LW_SEARCHSUBCOMMAND_INCLUDED

#include "subcommand.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwSearchSubCommand LwSearchSubCommand;
typedef struct _LwSearchSubCommandClass LwSearchSubCommandClass;
typedef struct _LwSearchSubCommandPrivate LwSearchSubCommandPrivate;
typedef struct _LwSearchSubCommandClassPrivate LwSearchSubCommandClassPrivate;

#define LW_TYPE_SEARCHSUBCOMMAND              (lw_searchsubcommand_get_type())
#define LW_SEARCHSUBCOMMAND(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_SEARCHSUBCOMMAND, LwSearchSubCommand))
#define LW_SEARCHSUBCOMMAND_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_SEARCHSUBCOMMAND, LwSearchSubCommandClass))
#define LW_IS_SEARCHSUBCOMMAND(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_SEARCHSUBCOMMAND))
#define LW_IS_SEARCHSUBCOMMAND_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_SEARCHSUBCOMMAND))
#define LW_SEARCHSUBCOMMAND_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_SEARCHSUBCOMMAND, LwSearchSubCommandClass))

struct _LwSearchSubCommand {
  LwSubCommand object;
};

struct _LwSearchSubCommandClass {
  LwSubCommandClass parent_class;
  LwSearchSubCommandClassPrivate *priv;
};

//Methods
LwSubCommand* lw_searchsubcommand_new (void);
GType lw_searchsubcommand_get_type (void) G_GNUC_CONST;

//Properties


G_END_DECLS

#endif

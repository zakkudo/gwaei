#ifndef W_CONSOLE_INCLUDED
#define W_CONSOLE_INCLUDED

#include <libwaei/command.h>
#include <waei/application.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _WCommand WCommand;
typedef struct _WCommandClass WCommandClass;
typedef struct _WCommandPrivate WCommandPrivate;
typedef struct _WCommandClassPrivate WCommandClassPrivate;

#define W_TYPE_COMMAND              (w_command_get_type())
#define W_COMMAND(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), W_TYPE_COMMAND, WCommand))
#define W_COMMAND_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), W_TYPE_COMMAND, WCommandClass))
#define W_IS_COMMAND(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), W_TYPE_COMMAND))
#define W_IS_COMMAND_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), W_TYPE_COMMAND))
#define W_COMMAND_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), W_TYPE_COMMAND, WCommandClass))

struct _WCommand {
  LwCommand object;
};

struct _WCommandClass {
  LwCommandClass parent_class;
  WCommandClassPrivate *priv;
};

//Methods

WCommand* w_command_new (WApplication *application, GApplicationCommandLine *command_line);
GType w_command_get_type (void) G_GNUC_CONST;

#endif

#ifndef W_CONSOLE_INCLUDED
#define W_CONSOLE_INCLUDED

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
  GObject object;
  WCommandPrivate *priv;
};

struct _WCommandClass {
  GObjectClass parent_class;
  WCommandClassPrivate *priv;
};

//Methods
WCommand* w_command_new (WApplication *application, GApplicationCommandLine *command_line);
GType w_command_get_type (void) G_GNUC_CONST;

gint w_command_run (WCommand *self);

void w_command_set_application (WCommand *self, WApplication *application);
WApplication* w_command_get_application (WCommand *self);

void w_command_set_command_line (WCommand *self, GApplicationCommandLine *command_line);
GApplicationCommandLine* w_command_get_command_line (WCommand *self);

void w_command_about (WCommand *self);
void w_command_list (WCommand *self);
void w_command_print_installable_dictionaries (WCommand *self);
void w_command_print_available_dictionaries (WCommand *self);
gint w_command_search (WCommand *self);
gint w_command_install_dictionary (WCommand *self);
gint w_command_uninstall_dictionary (WCommand *self);

const gchar* w_command_get_install_switch_data (WCommand *self);
const gchar* w_command_get_uninstall_switch_data (WCommand *self);

void w_command_progress_primary_message_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);
void w_command_progress_secondary_message_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);
void w_command_progress_completed_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);
void w_command_progress_fraction_changed_cb (WCommand *self, GParamSpec *pspec, LwProgress *progress);

void w_command_watch_progress (WCommand *self, LwProgress *progress);
void w_command_unwatch_current_progress (WCommand *self);

#include "command-output.h"
#include "command-callbacks.h"

#endif

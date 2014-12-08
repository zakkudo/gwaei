#ifndef W_CONSOLE_INCLUDED
#define W_CONSOLE_INCLUDED

#include <libwaei/command.h>

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
  WCommandPrivate *priv;
};

struct _WCommandClass {
  LwCommandClass parent_class;
  WCommandClassPrivate *priv;
};

//Methods

WCommand* w_command_new (WApplication *application, GApplicationCommandLine *command_line);
GType w_command_get_type (void) G_GNUC_CONST;

gint w_command_run (WCommand *self);

void w_command_about (WCommand *self);
void w_command_list (WCommand *self);
void w_command_print_installable_dictionaries (WCommand *self);
void w_command_print_available_dictionaries (WCommand *self);
gint w_command_search (WCommand *self);
gint w_command_install_dictionary (WCommand *self);
gint w_command_uninstall_dictionary (WCommand *self);

//Properties

void w_command_set_dictionary_install_switch_text (WCommand *self, const gchar *dictionary_install_switch_text);
const gchar* w_command_get_dictionary_install_switch_text (WCommand *self);
gboolean w_command_has_dictionary_install_request (WCommand *self);

void w_command_set_dictionary_uninstall_switch_text (WCommand *self, const gchar *dictionary_uninstall_switch_text);
const gchar* w_command_get_dictionary_uninstall_switch_text (WCommand *self);
gboolean w_command_has_dictionary_uninstall_request (WCommand *self);

void w_command_set_dictionary_switch_text (WCommand *self, const gchar *dictionary_switch_text);
const gchar* w_command_get_dictionary_switch_text (WCommand *self);

void w_command_set_query_switch_text (WCommand *self, const gchar *query_switch_text);
const gchar* w_command_get_query_switch_text (WCommand *self);
gboolean w_command_has_query_request (WCommand *self);

void w_command_set_quiet_switch (WCommand *self, gboolean quiet_switch);
gboolean w_command_get_quiet_switch (WCommand *self);

void w_command_set_exact_switch (WCommand *self, gboolean exact_switch);
gboolean w_command_get_exact_switch (WCommand *self);

void w_command_set_list_switch (WCommand *self, gboolean list_switch);
gboolean w_command_get_list_switch (WCommand *self);

void w_command_set_version_switch (WCommand *self, gboolean version_switch);
gboolean w_command_get_version_switch (WCommand *self);

void w_command_set_color_switch (WCommand *self, gboolean color_switch);
gboolean w_command_get_color_switch (WCommand *self);

void w_command_set_force_index_rebuild_switch (WCommand *self, gboolean force_index_rebuild_switch);
gboolean w_command_get_force_index_rebuild_switch (WCommand *self);

void w_command_set_start_server_switch (WCommand *self, gboolean start_server_switch);
gboolean w_command_get_start_server_switch (WCommand *self);


#include "command-output.h"

#endif

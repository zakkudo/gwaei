#ifndef W_COMMAND_PRIVATE_INCLUDED
#define W_COMMAND_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_APPLICATION,
  PROP_COMMAND_LINE,
  PROP_PROGRESS,
  PROP_DICTIONARY_SWITCH_TEXT,
  PROP_DICTIONARY_INSTALL_SWITCH_TEXT,
  PROP_DICTIONARY_UNINSTALL_SWITCH_TEXT,
  PROP_QUERY_SWITCH_TEXT,
  PROP_QUIET_SWITCH,
  PROP_EXACT_SWITCH,
  PROP_LIST_SWITCH,
  PROP_VERSION_SWITCH,
  PROP_COLOR_SWITCH,
  TOTAL_PROPS
} Props;

typedef enum {
  SIGNALID_PROGRESS_PRIMARY_MESSAGE_CHANGED,
  SIGNALID_PROGRESS_SECONDARY_MESSAGE_CHANGED,
  SIGNALID_PROGRESS_COMPLETED_CHANGED,
  SIGNALID_PROGRESS_FRACTION_CHANGED,
  TOTAL_SIGNALIDS
} SignalId;

struct _Config {
};

struct _Data {
  GOptionContext *context;
  LwDictionary *dictionary;
  WApplication *application;
  GApplicationCommandLine *command_line;
  GMainLoop *loop;

  LwProgress *progress;
  guint signalid[TOTAL_SIGNALIDS];
};

struct _Argument {
  gboolean quiet_switch;
  gboolean exact_switch;
  gboolean list_switch;
  gboolean version_switch;
  gboolean color_switch;
  gboolean rebuild_index;
  gboolean start_server;

  gchar* dictionary_switch_text;
  gchar* dictionary_install_switch_text;
  gchar* dictionary_uninstall_switch_text;
  gchar* query_switch_text;
};

struct _WCommandPrivate {
  gint* argc;
  gchar*** argv;
  
  struct _Config config;
  struct _Data data;
  struct _Argument argument;
};

struct _WCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define W_COMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), W_TYPE_COMMAND, WCommandPrivate))

G_END_DECLS

#include "command-callbacks.h"

#endif

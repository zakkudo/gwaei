#ifndef W_COMMAND_PRIVATE_INCLUDED
#define W_COMMAND_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
  PROP_0,
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
  PROP_FORCE_INDEX_REBUILD_SWITCH,
  PROP_START_SERVER_SWITCH,
  TOTAL_PROPS
} Props;

typedef enum {
  SIGNALID_PROGRESS_PRIMARY_MESSAGE_CHANGED,
  SIGNALID_PROGRESS_SECONDARY_MESSAGE_CHANGED,
  SIGNALID_PROGRESS_COMPLETED_CHANGED,
  SIGNALID_PROGRESS_FRACTION_CHANGED,
  TOTAL_SIGNALIDS
} SignalId;

struct _Argument {
  gboolean quiet_switch;
  gboolean exact_switch;
  gboolean list_switch;
  gboolean version_switch;
  gboolean color_switch;
  gboolean force_index_rebuild_switch;
  gboolean start_server_switch;

  gchar* dictionary_switch_text;
  gchar* dictionary_install_switch_text;
  gchar* dictionary_uninstall_switch_text;
  gchar* query_switch_text;
};

struct _WCommandPrivate {
  LwDictionary *dictionary;
  GMainLoop *loop;
  LwProgress *progress;
  guint signalid[TOTAL_SIGNALIDS];
  struct _Argument argument;
};

struct _WCommandClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define W_COMMAND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), W_TYPE_COMMAND, WCommandPrivate))

//Methods

void w_command_watch_progress (WCommand *self, LwProgress *progress);
void w_command_unwatch_current_progress (WCommand *self);


G_END_DECLS

#include "command-callbacks.h"

#endif

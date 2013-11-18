#ifndef GW_APPLICATION_PRIVATE_INCLUDED
#define GW_APPLICATION_PRIVATE_INCLUDED

#include <gwaei/vocabularyliststore.h>

G_BEGIN_DECLS

typedef enum {
  GW_APPLICATION_SIGNALID_MATCH_FG,
  GW_APPLICATION_SIGNALID_MATCH_BG,
  GW_APPLICATION_SIGNALID_HEADER_FG,
  GW_APPLICATION_SIGNALID_HEADER_BG,
  GW_APPLICATION_SIGNALID_COMMENT_FG,
  TOTAL_GW_APPLICATION_SIGNALIDS
} GwApplicationSignalId;

struct _Config {
  GOptionContext *context;
  gint block_new_searches;
  gchar   *arg_dictionary;
  gchar   *arg_query;
  gboolean arg_version_switch;
  gboolean arg_new_vocabulary_window_switch;
};

struct _Data {
  LwMorphologyEngine *morphologyengine;
  GtkListStore *vocabularyliststore;
  LgwDictionaryList *installed_dictionarylist;
  LgwDictionaryList *installable_dictionarylist;
  LwPreferences *preferences;
};

struct _GwApplicationPrivate {
  gint* argc;
  gchar*** argv;

  guint signalid[TOTAL_GW_APPLICATION_SIGNALIDS];
  struct _Data data;
  struct _Config config;
};

#define GW_APPLICATION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GW_TYPE_APPLICATION, GwApplicationPrivate))

G_END_DECLS

#endif

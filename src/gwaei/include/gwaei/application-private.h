#ifndef GW_APPLICATION_PRIVATE_INCLUDED
#define GW_APPLICATION_PRIVATE_INCLUDED

#include <gwaei/vocabularyliststore.h>

G_BEGIN_DECLS


struct _Arguments {
  gchar   *dictionary;
  gchar   *query;
  gboolean version_switch;
  gboolean new_vocabulary_window_switch;
};

struct _Config {
  GOptionContext *context;
  gint block_new_searches;
  struct _Arguments arguments;
  LwPreferences *preferences;
};

struct _DictionaryList {
  LgwDictionaryList *installed;
  LgwDictionaryList *installable;
};

struct _Data {
  LwMorphologyEngine *morphologyengine;
  GtkListStore *vocabularyliststore;
  struct _DictionaryList dictionarylist;
};

struct _GwApplicationPrivate {
  gint* argc;
  gchar*** argv;

  struct _Data data;
  struct _Config config;
};

#define GW_APPLICATION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GW_TYPE_APPLICATION, GwApplicationPrivate))

G_END_DECLS

#endif

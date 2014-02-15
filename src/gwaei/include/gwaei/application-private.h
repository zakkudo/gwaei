#ifndef GW_APPLICATION_PRIVATE_INCLUDED
#define GW_APPLICATION_PRIVATE_INCLUDED

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

struct _DictionaryListStore {
  LgwDictionaryListStore *installed;
  LgwDictionaryListStore *installable;
};

struct _Data {
  LwMorphologyEngine *morphologyengine;
  LgwVocabularyListStore *vocabulary_list_store;
  struct _DictionaryListStore dictionary_list_store;
};

struct _GwApplicationPrivate {
  gint* argc;
  gchar*** argv;

  struct _Data data;
  struct _Config config;
};

#define GW_APPLICATION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GW_TYPE_APPLICATION, GwApplicationPrivate))

void gw_application_set_preferences (GwApplication *self, LwPreferences *preferences);

G_END_DECLS

#endif

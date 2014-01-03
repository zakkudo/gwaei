#ifndef W_APPLICATION_PRIVATE_INCLUDED
#define W_APPLICATION_PRIVATE_INCLUDED

G_BEGIN_DECLS

struct _Config {
  LwPreferences *preferences;
};

struct _WApplicationPrivate {
  gint* argc;
  gchar*** argv;
  
  struct _Config config;

  LwDictionaryList *installed_dictionarylist;
  LwDictionaryList *installable_dictionarylist;
  LwMorphologyEngine *morphologyengine;

  gboolean arg_quiet_switch;
  gboolean arg_exact_switch;
  gboolean arg_list_switch;
  gboolean arg_version_switch;
  gboolean arg_color_switch;
  gboolean arg_rebuild_index;

  gchar* arg_dictionary_switch_data;
  gchar* arg_install_switch_data;
  gchar* arg_uninstall_switch_data;
  gchar* arg_query_text_data;

  GOptionContext *context;
};

#define W_APPLICATION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), W_TYPE_APPLICATION, WApplicationPrivate))

void w_application_set_preferences (WApplication *application, LwPreferences *preferences);

G_END_DECLS

#endif

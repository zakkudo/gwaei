#ifndef LW_DICTIONARYINSTALL_PRIVATE_INCLUDED
#define LW_DICTIONARYINSTALL_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_NAME,
    PROP_DESCRIPTION,
    PROP_GTYPE,
    PROP_TEXT_ENCODING,
    PROP_PREFERENCES,
    PROP_DOWNLOAD_PREFERENCE_KEY,
    PROP_DOWNLOAD_URI,
    PROP_SPLIT_PLACES_FROM_NAMES,
    PROP_MERGE_RADICALS_INTO_KANJI,
    PROP_PROGRESS,
    PROP_DEPENDENCIES,
    TOTAL_PROPS
} Props;

typedef enum {
    CLASS_SIGNALID_UNUSED,
    TOTAL_CLASS_SIGNALIDS
} SignalId;


struct _Data {
  gchar *name;
  gchar *description;
  LwDictionaryInstallStatus status;
  LwPreferences *preferences;
  gchar *download_key;
  gchar *text_encoding;
  gchar **dependencies;
  GType gtype;
  LwProgress *progress;
};

struct _Config {
  gboolean merge_radicals_into_kanji;
  gboolean split_places_from_names;
};

struct _LwDictionaryInstallPrivate {
  struct _Data data;
  struct _Config config;
};


struct _LwDictionaryInstallClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_DICTIONARYINSTALL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYINSTALL, LwDictionaryInstallPrivate));

G_END_DECLS

#endif




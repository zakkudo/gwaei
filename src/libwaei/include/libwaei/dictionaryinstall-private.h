#ifndef LW_DICTIONARYINSTALL_PRIVATE_INCLUDED
#define LW_DICTIONARYINSTALL_PRIVATE_INCLUDED

#include "dictionaryinstallstatehistory.h"

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_ID,
    PROP_NAME,
    PROP_DESCRIPTION,
    PROP_GTYPE,
    PROP_TEXT_ENCODING,
    PROP_PREFERENCES,
    PROP_DOWNLOAD_KEY,
    PROP_DOWNLOAD_URI,
    PROP_INSTALL_PATH,
    PROP_SPLIT_PLACES_FROM_NAMES,
    PROP_MERGE_RADICALS_INTO_KANJI,
    PROP_PROGRESS,
    PROP_STATUS,
    PROP_DEPENDENCIES,
    TOTAL_PROPS
} Props;

typedef enum {
    CLASS_SIGNALID_UNUSED,
    TOTAL_CLASS_SIGNALIDS
} SignalId;

struct _Index {
  GHashTable *id;
};

struct _LwDictionaryInstallPrivate {
  struct _Index index;
  gchar *id;
  gchar *name;
  gchar *description;
  LwPreferences *preferences;
  gchar *download_key;
  gchar *download_uri;
  gchar *text_encoding;
  GList *dependancies;
  GType gtype;
  LwProgress *progress;
  LwDictionaryInstallStateHistory *history;
  gchar *install_path;

  gboolean merge_radicals_into_kanji;
  gboolean split_places_from_names;
};


struct _LwDictionaryInstallClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_DICTIONARYINSTALL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYINSTALL, LwDictionaryInstallPrivate));

const gchar* lw_dictionaryinstall_get_name (LwDictionaryInstall *self);
void lw_dictionaryinstall_set_name (LwDictionaryInstall *self, const gchar *NAME);


G_END_DECLS

#endif




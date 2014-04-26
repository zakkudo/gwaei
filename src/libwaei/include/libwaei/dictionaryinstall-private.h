#ifndef LW_DICTIONARYINSTALL_PRIVATE_INCLUDED
#define LW_DICTIONARYINSTALL_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum {
    PROP_0,
    PROP_PROGRESS,
    TOTAL_PROPS
} Props;

typedef enum {
    CLASS_SIGNALID_UNUSED,
    TOTAL_CLASS_SIGNALIDS
} SignalId;


struct _Data {
};

struct _Config {
};

struct _LwDictionaryInstallPrivate {
  struct _Data data;
  struct _Config config;
};


/*TODO
struct _LwDictionaryInstall {
  gchar *name;
  gchar *description;

  LwDictionaryInstallerStatus status;
  gint index;

  gchar *files;
  gchar *downloads;

  gchar **filelist;
  gchar **downloadlist;
  gchar **decompresslist;
  gchar **encodelist;
  gchar **postprocesslist;
  gchar **installlist;
  gchar **installedlist;

  LwPreferences *preferences;
  const gchar *key;
  gboolean builtin;
  gulong listenerid;            //!< An id to hold the g_signal_connect value when the source copy uri pref is set
  LwEncoding encoding;          //!< Path to the raw unziped dictionary file
  gboolean postprocess;
};
*/


struct _LwDictionaryInstallClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

#define LW_DICTIONARYINSTALL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYINSTALL, LwDictionaryInstallPrivate));

G_END_DECLS

#endif




#ifndef LW_DICTIONARY_PRIVATE_INCLUDED
#define LW_DICTIONARY_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_FILENAME,
  PROP_MORPHOLOGYENGINE
} LwDictionaryProps;

typedef enum {
  CLASS_SIGNALID_PROGRESS_CHANGED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwDictionaryPrivate {
    gchar *name;
    gchar *filename;
    LwIndex *index;
    LwDictionaryData *data;
    LwMorphologyEngine *morphologyengine;
    gdouble progress;
    size_t length;       //!< Length of the file
    GMutex mutex;
    LwDictionaryInstall *install;
    gboolean selected;
};

struct _LwDictionaryClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];

  //Virtual methods
  gint (*parse_result) (LwDictionary *dictionary, LwResult *result, const gchar* TEXT);
  gboolean (*installer_postprocess) (LwDictionary *dictionary, gchar** sourcelist, gchar** targetlist, LwProgress*);
};

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

#define LW_DICTIONARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARY, LwDictionaryPrivate));

LwPreferences* lw_dictionarylist_get_preferences (LwDictionaryList *dictionary_list);
void lw_dictionarylist_set_preferences (LwDictionaryList *dictionary_listi, LwPreferences *preferences);

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>

#endif

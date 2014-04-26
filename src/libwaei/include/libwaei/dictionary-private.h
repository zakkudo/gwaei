#ifndef LW_DICTIONARY_PRIVATE_INCLUDED
#define LW_DICTIONARY_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_FILENAME,
  PROP_NAME,
  PROP_MORPHOLOGYENGINE,
  PROP_PROGRESS,
  TOTAL_PROPS
} LwDictionaryProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _Data {
  LwMorphologyEngine *morphology_engine;
  LwIndex *index;
  LwDictionaryData *data;
  GMutex mutex;
  gchar *name;
  LwProgress *progress;
};

struct _Config {
  gchar *filename;
  gboolean selected;
};

struct _LwDictionaryPrivate {
  struct _Data data;
  struct _Config config;
};

struct _LwDictionaryClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];

  //Virtual methods
  LwResult* (*parse) (LwDictionary *dictionary, const gchar* TEXT);
  gboolean (*installer_postprocess) (LwDictionary *dictionary, gchar** sourcelist, gchar** targetlist, LwProgress*);
};

#define LW_DICTIONARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARY, LwDictionaryPrivate));

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>

#endif


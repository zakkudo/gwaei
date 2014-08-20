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
  PROP_ID,
  PROP_PATH,
  PROP_BUFFERTREE,
  PROP_LENGTH,
  PROP_CHECKSUM,
  PROP_CONTENTS,
  TOTAL_PROPS
} LwDictionaryProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _Data {
  gchar *path;
  GTree *buffers;

  LwMorphologyEngine *morphology_engine;
  GMutex mutex;
  gchar *name;
  LwProgress *progress;

  gchar *filename;
  gchar *id;

  GMappedFile *mapped_file;
  gchar *contents;
  gchar *checksum;
};

struct _Config {
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
  LwDictionaryBufferParseFunc parse;
};

#define LW_DICTIONARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARY, LwDictionaryPrivate));

//Properties

void lw_dictionary_take_checksum (LwDictionary *self, gchar *checksum);
void lw_dictionary_sync_checksum (LwDictionary *self);

void lw_dictionary_take_contents (LwDictionary *self, gchar *contents);
void lw_dictionary_sync_contents (LwDictionary *self, LwProgress *progress);

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>

#endif


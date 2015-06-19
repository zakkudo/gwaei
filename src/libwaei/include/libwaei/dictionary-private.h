#ifndef LW_DICTIONARY_PRIVATE_INCLUDED
#define LW_DICTIONARY_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_FILENAME,
  PROP_NAME,
  PROP_PROGRESS,
  PROP_ID,
  PROP_PATH,
  PROP_CACHETREE,
  PROP_CHECKSUM,
  PROP_CONTENTS,
  PROP_CONTENT_LENGTH,
  PROP_CACHEFILE,
  TOTAL_PROPS
} LwDictionaryProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwDictionaryPrivate {
  gchar *path;
  GTree *caches;

  GMutex mutex;
  gchar *name;
  LwProgress *progress;

  gchar *filename;
  gchar *id;

  LwCacheFile *cachefile;
};

struct _LwDictionaryClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];

  gchar *install_path;
};

#define LW_DICTIONARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARY, LwDictionaryPrivate));

//Properties

static LwCacheFile* lw_dictionary_get_cachefile (LwDictionary * self);
static void lw_dictionary_set_cachefile (LwDictionary * self, LwCacheFile * cachefile);

static void lw_dictionary_sync_cachefile (LwDictionary * self);

static void lw_dictionary_sync_path (LwDictionary * self);
static gchar * lw_dictionary_build_path (LwDictionary * self);

static void lw_dictionary_sync_id (LwDictionary * self);

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>

#endif


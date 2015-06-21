#ifndef LW_DICTIONARY_PRIVATE_INCLUDED
#define LW_DICTIONARY_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_NAME,
  PROP_PROGRESS,
  PROP_ID,
  PROP_CONTENTS_PATH,
  PROP_CONTENTS_FILENAME,
  PROP_CONTENTS_CHECKSUM,
  PROP_CONTENTS_CACHEFILE,
  PROP_CONTENTS,
  PROP_CONTENT_LENGTH,
  TOTAL_PROPS
} LwDictionaryProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwDictionaryPrivate {
  gchar * name;
  gchar * id;
  GMutex mutex;
  LwProgress * progress;

  GTree * parsed_cachetree;

  gchar * contents_path;
  gchar * contents_filename;
  LwCacheFile * contents_cachefile;
};

struct _LwDictionaryClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];

  gchar *install_path;
};

#define LW_DICTIONARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARY, LwDictionaryPrivate));

//Properties

static LwCacheFile* lw_dictionary_get_contents_cachefile (LwDictionary * self);
static void lw_dictionary_set_contents_cachefile (LwDictionary * self, LwCacheFile * cachefile);

static void lw_dictionary_sync_contents_cachefile (LwDictionary * self);

static void lw_dictionary_sync_contents_path (LwDictionary * self);
static gchar * lw_dictionary_build_contents_path (LwDictionary * self);

static void lw_dictionary_sync_id (LwDictionary * self);

static LwParsed* lw_dictionary_parse (LwDictionary * self, LwCacheFile * cache_file, LwProgress * progress);

static GTree * lw_dictionary_get_parsed_cachetree (LwDictionary * self);
static void lw_dictionary_set_parsed_cachetree (LwDictionary * self, GTree * tree);

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>

#endif


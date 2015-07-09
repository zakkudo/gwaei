#ifndef LW_DICTIONARY_PRIVATE_INCLUDED
#define LW_DICTIONARY_PRIVATE_INCLUDED

#include "dictionarycachetree.h"

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

  LwDictionaryCacheTree * cachetree;

  gchar * contents_path;
  gchar * contents_filename;
  LwMappedFile * contents_mappedfile;
  gchar * contents_checksum;
};

struct _LwDictionaryClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];

  gchar *install_path;
};

#define LW_DICTIONARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARY, LwDictionaryPrivate));

//Properties

static LwMappedFile* lw_dictionary_get_contents_mappedfile (LwDictionary * self);
static void lw_dictionary_set_contents_mappedfile (LwDictionary * self, LwMappedFile * mapped_file);

static void lw_dictionary_sync_contents (LwDictionary * self);

static void lw_dictionary_sync_contents_path (LwDictionary * self);
static gchar * lw_dictionary_build_contents_path (LwDictionary * self, gchar const * FILENAME);

static void lw_dictionary_sync_id (LwDictionary * self);

static LwParsed* lw_dictionary_parse (LwDictionary * self, LwCacheFile * cache_file, LwProgress * progress);

static LwDictionaryCacheTree * lw_dictionary_get_cachetree (LwDictionary * self);
static void lw_dictionary_set_cachetree (LwDictionary * self, LwDictionaryCacheTree * tree);

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>

#endif


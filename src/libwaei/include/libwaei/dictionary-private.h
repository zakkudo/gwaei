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
  PROP_CONTENTS_MAPPEDFILE,
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

  LwMappedFile *contents_mappedfile;
  gchar *checksum;
};

struct _LwDictionaryClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];

  gchar *install_path;

  //Virtual methods
  LwDictionaryParseFunc parse;
  gint (* get_total_columns) (LwDictionary * self);
  gchar const * (* get_column_language) (LwDictionary * self, gint column_num);
  LwDictionaryColumnHandling (* get_column_handling) (LwDictionary * self, gint column_num);
};

#define LW_DICTIONARY_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARY, LwDictionaryPrivate));

//Properties

static void lw_dictionary_set_checksum (LwDictionary * self, gchar const * CHECKSUM);
static void lw_dictionary_sync_checksum (LwDictionary * self);

static void lw_dictionary_set_contents_mappedfile (LwDictionary * self, LwMappedFile * contents_mappedfile);

static void lw_dictionary_sync_contents_mappedfile (LwDictionary * self);

static void lw_dictionary_sync_path (LwDictionary * self);
static gchar * lw_dictionary_build_path (LwDictionary * self);

static void lw_dictionary_sync_id (LwDictionary * self);

G_END_DECLS

#include <libwaei/dictionary-callbacks.h>

#endif


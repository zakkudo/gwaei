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
  PROP_CONTENT_LENGTH,
  PROP_CHECKSUM,
  PROP_CONTENTS,
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

  GMappedFile *mapped_file;
  gchar *contents;
  gchar *checksum;
};

struct _LwDictionaryClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];

  //Virtual methods
  LwDictionaryParseFunc parse;
  gint (* get_total_columns) (LwDictionary *self);
  gchar const * (* get_column_language) (LwDictionary *self, gint column_num);
  LwDictionaryColumnHandling (* get_column_handling) (LwDictionary *self, gint column_num);
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


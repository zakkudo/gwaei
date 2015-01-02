#ifndef LW_CACHEFILE_PRIVATE_INCLUDED
#define LW_CACHEFILE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_PATH,
  PROP_CHECKSUM,
  PROP_CONTENTS,
  PROP_CONTENT_LENGTH,
  TOTAL_PROPS
} LwCacheFileProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwCacheFilePrivate {
  FILE * stream;
  gchar const * CHECKSUM;
  gsize content_length;
  gchar * contents;
  gchar * path;
};

struct _LwCacheFileClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_CACHEFILE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_CACHEFILE, LwCacheFilePrivate));

static FILE* _ensure_fopen (LwCacheFile *self, GError **error);
void static _ensure_fclose (LwCacheFile *self, GError **error);

static void lw_cachefile_set_path (LwCacheFile * self, gchar const * PATH);

//Properties

G_END_DECLS

#endif


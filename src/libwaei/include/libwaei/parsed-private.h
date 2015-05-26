#ifndef LW_PARSED_PRIVATE_INCLUDED
#define LW_PARSED_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_CACHEFILE,
  TOTAL_PROPS
} Prop;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwParsedPrivate {
  gsize num_lines; //! < The number of lines
  LwParsedLine *lines; //! < A set of categorized strvs
  LwCacheFile *cache_file;
};

struct _LwParsedClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_PARSED_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_PARSED, LwParsedPrivate));

static gsize lw_parsed_serialize (LwParsed *self, gchar *preallocated_buffer, LwProgress *progress);
static gsize lw_parsed_deserialize_into (LwParsed *self, const gchar *serialized_data, gsize serialized_length, LwProgress *progress);

static void lw_parsed_set_cachefile (LwParsed * self, LwCacheFile * cache_file);

G_END_DECLS

#endif


#ifndef LW_PARSED_PRIVATE_INCLUDED
#define LW_PARSED_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  TOTAL_PROPS
} LwParsedProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwParsedPrivate {
  gsize num_lines; //! < The number of lines
  gchar *contents_reference_pointer; //! < The reference point of each strv
  gsize content_length;
  LwParsedLine *lines; //! < A set of categorized strvs
  LwCacheFile *contents_cachefile;
};

#define LW_PARSED_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_PARSED, LwParsedPrivate));

G_END_DECLS

#endif


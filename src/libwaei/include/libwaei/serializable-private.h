#ifndef LW_SERIALIZABLE_PRIVATE_INCLUDED
#define LW_SERIALIZABLE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  TOTAL_PROPS
} LwSerializableProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwSerializablePrivate {
  LwCacheFile *cache_file;
};

#define LW_SERIALIZABLE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_SERIALIZABLE, LwSerializablePrivate));

G_END_DECLS

#endif


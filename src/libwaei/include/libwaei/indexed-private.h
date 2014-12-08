#ifndef LW_INDEXED_PRIVATE_INCLUDED
#define LW_INDEXED_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  TOTAL_PROPS
} LwIndexedProps;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwIndexedPrivate {
  LwParsed *parsed;
};

#define LW_INDEXED_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_INDEXED, LwIndexedPrivate));

G_END_DECLS

#endif


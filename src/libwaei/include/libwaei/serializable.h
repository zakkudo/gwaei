#ifndef LW_SERIALIZABLE_INCLUDED
#define LW_SERIALIZABLE_INCLUDED

#include "cachefile.h"

G_BEGIN_DECLS

#include <glib-object.h>

#define LW_TYPE_SERIALIZABLE                (lw_serializable_get_type ())
#define LW_SERIALIZABLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_SERIALIZABLE, LwSerializable))
#define LW_IS_SERIALIZABLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_SERIALIZABLE))
#define LW_SERIALIZABLE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), LW_TYPE_SERIALIZABLE, LwSerializableInterface))

typedef struct _LwSerializable LwSerializable; /* dummy object */
typedef struct _LwSerializableInterface LwSerializableInterface;

struct _LwSerializableInterface {
      GTypeInterface parent;
      GList* (*serialize) (LwSerializable *self, gchar *preallocated_buffer, LwProgress *progress);
      void (*deserialize_into) (LwSerializable *self, gchar const *serialized_data, gsize serialized_length, LwProgress *progress);
};

GType lw_serializable_get_type (void);


gsize lw_serializable_get_serialized_length (LwSerializable *self, LwProgress *progress);

gsize lw_serializable_serialize (LwSerializable *self, gchar *preallocated_buffer, LwProgress *progress);
gsize lw_serializable_serialize_to_cachefile (LwSerializable *self, gchar const *CHECKSUM, LwCacheFile *cachefile, LwProgress *progress);

gsize lw_serializable_deserialize_into (LwSerializable *self, gchar const *serialized_data, gsize serialized_length, LwProgress *progress);
gsize lw_serializable_deserialize_from_cachefile_into (LwSerializable *self, gchar const *EXPECTED_CHECKSUM, LwCacheFile *cachefile, LwProgress *progress);


G_END_DECLS
    
#endif


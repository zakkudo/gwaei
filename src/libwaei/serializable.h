#ifndef LW_SERIALIZABLE_INCLUDED
#define LW_SERIALIZABLE_INCLUDED

#include <glib-object.h>
#include "progress.h"
#include "cache-file.h"

G_BEGIN_DECLS

#define LW_TYPE_SERIALIZABLE lw_serializable_get_type ()
G_DECLARE_DERIVABLE_TYPE (LwSerializable, lw_serializable, LW, SERIALIZABLE, GObject)

struct _LwSerializableClass {
    GObjectClass parent_class;
    gsize (*serialize) (LwSerializable *self, gchar *preallocated_buffer, LwProgress *progress);
    gsize (*deserialize_into) (LwSerializable *self, gchar const *serialized_data, gsize serialized_length, LwProgress *progress);
};

// Methods

gsize lw_serializable_get_serialized_length (LwSerializable *self, LwProgress *progress);

gsize lw_serializable_serialize (LwSerializable *self, gchar *preallocated_buffer, LwProgress *progress);
gsize lw_serializable_serialize_to_cache_file (LwSerializable *self, gchar const *CHECKSUM, LwCacheFile *cache_file, LwProgress *progress);

gsize lw_serializable_deserialize_into (LwSerializable *self, gchar const *serialized_data, gsize serialized_length, LwProgress *progress);
gsize lw_serializable_deserialize_from_cache_file_into (LwSerializable *self, gchar const *EXPECTED_CHECKSUM, LwCacheFile *cache_file, LwProgress *progress);


G_END_DECLS
    
#endif


#ifndef LW_SERIALIZABLE_INCLUDED
#define LW_SERIALIZABLE_INCLUDED

#include "progress.h"
#include "cachefile.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwSerializable LwSerializable;
typedef struct _LwSerializableClass LwSerializableClass;
typedef struct _LwSerializablePrivate LwSerializablePrivate;

#define LW_TYPE_SERIALIZABLE              (lw_serializable_get_type())
#define LW_SERIALIZABLE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_SERIALIZABLE, LwSerializable))
#define LW_SERIALIZABLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_SERIALIZABLE, LwSerializableClass))
#define LW_IS_SERIALIZABLE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_SERIALIZABLE))
#define LW_IS_SERIALIZABLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_SERIALIZABLE))
#define LW_SERIALIZABLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_SERIALIZABLE, LwSerializableClass))

struct _LwSerializable {
    GObject object;
    LwSerializablePrivate *priv;
};

struct _LwSerializableClass {
    GObjectClass parent_class;
    gsize (*serialize) (LwSerializable *self, gchar *preallocated_buffer, LwProgress *progress);
    gsize (*deserialize_into) (LwSerializable *self, gchar const *serialized_data, gsize serialized_length, LwProgress *progress);
};

GType lw_serializable_get_type (void);

gsize lw_serializable_get_serialized_length (LwSerializable *self, LwProgress *progress);

gsize lw_serializable_serialize (LwSerializable *self, gchar *preallocated_buffer, LwProgress *progress);
gsize lw_serializable_serialize_to_cachefile (LwSerializable *self, gchar const *CHECKSUM, LwCacheFile *cachefile, LwProgress *progress);

gsize lw_serializable_deserialize_into (LwSerializable *self, gchar const *serialized_data, gsize serialized_length, LwProgress *progress);
gsize lw_serializable_deserialize_from_cachefile (LwSerializable *self, gchar const *EXPECTED_CHECKSUM, LwCacheFile *cachefile, LwProgress *progress);


G_END_DECLS
    
#endif


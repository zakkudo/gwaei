#ifndef LW_INDEXED_INCLUDED
#define LW_INDEXED_INCLUDED 

#include "parsed.h"
#include "progress.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwIndexed LwIndexed;
typedef struct _LwIndexedClass LwIndexedClass;
typedef struct _LwIndexedPrivate LwIndexedPrivate;

#define LW_TYPE_INDEXED              (lw_indexed_get_type())
#define LW_INDEXED(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_INDEXED, LwIndexed))
#define LW_INDEXED_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_INDEXED, LwIndexedClass))
#define LW_IS_INDEXED(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_INDEXED))
#define LW_IS_INDEXED_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_INDEXED))
#define LW_INDEXED_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_INDEXED, LwIndexedClass))

struct _LwIndexed {
  LwSerializable object;
  LwIndexedPrivate *priv;
};

struct _LwIndexedClass {
  LwSerializableClass parent_class;
};


LwIndexed* lw_indexed_new (LwParsed *parsed);

GType lw_indexed_get_type (void) G_GNUC_CONST;

gsize lw_indexed_serialize (LwIndexed *self, gchar *preallocated_buffer, LwProgress *progress);
gsize lw_indexed_deserialize_into (LwIndexed *self, const gchar *serialized_data, gsize sereialized_length, LwProgress *progress);

G_END_DECLS

#endif



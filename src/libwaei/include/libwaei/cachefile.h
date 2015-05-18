#ifndef LW_CACHEFILE_INCLUDED
#define LW_CACHEFILE_INCLUDED

#include "mappedfile.h"
#include "progress.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwCacheFile LwCacheFile;
typedef struct _LwCacheFileClass LwCacheFileClass;
typedef struct _LwCacheFilePrivate LwCacheFilePrivate;
typedef struct _LwCacheFileClassPrivate LwCacheFileClassPrivate;

#define LW_TYPE_CACHEFILE              (lw_cachefile_get_type())
#define LW_CACHEFILE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_CACHEFILE, LwCacheFile))
#define LW_CACHEFILE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_CACHEFILE, LwCacheFileClass))
#define LW_IS_CACHEFILE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_CACHEFILE))
#define LW_IS_CACHEFILE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_CACHEFILE))
#define LW_CACHEFILE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_CACHEFILE, LwCacheFileClass))

struct _LwCacheFile {
  LwMappedFile object;
  LwCacheFilePrivate *priv;
};

struct _LwCacheFileClass {
  LwMappedFileClass parent_class;
  LwCacheFileClassPrivate *priv;
};

#define LW_CACHEFILE_ERROR lw_cachefile_error_quark ()

typedef enum {
  LW_CACHEFILE_ERRORCODE_INVALID_CHECKSUM,
  LW_CACHEFILE_ERRORCODE_CORRUPT_CONTENTS
} LwCacheFileErrorCode;

//Methods
LwCacheFile * lw_cachefile_new (gchar const * PATH);
GType lw_cachefile_get_type (void) G_GNUC_CONST;
gsize lw_cachefile_write (LwCacheFile *self, const gchar *CHECKSUM, const gchar *CONTENTS, gssize content_length, LwProgress  *progress);
gsize lw_cachefile_write_cachefile (LwCacheFile  * self, const gchar * CHECKSUM, LwCacheFile * cache_file, LwProgress * progress);
gsize lw_cachefile_read (LwCacheFile *self, const gchar *EXPECTED_CHECKSUM, LwProgress *progress);

//Properties
const gchar* lw_cachefile_get_checksum (LwCacheFile *self);
gchar* lw_cachefile_get_contents (LwCacheFile *self);
gsize lw_cachefile_length (LwCacheFile *self);


G_END_DECLS

#endif

#ifndef LW_CACHEFILE_INCLUDED
#define LW_CACHEFILE_INCLUDED

G_BEGIN_DECLS

typedef struct _LwCacheFile LwCacheFile;

//Methods
LwCacheFile* lw_cachefile_new (const gchar *FILENAME);
GType lw_cachefile_get_type (void);

LwCacheFile *lw_cachefile_ref (LwCacheFile *self);
void lw_cachefile_unref (LwCacheFile *self);

const gchar* lw_cachefile_get_checksum (LwCacheFile *self);

const gchar* lw_cachefile_get_contents (LwCacheFile *self);

void lw_cachefile_write (LwCacheFile *self, const gchar *CHECKSUM, const gchar *CONTENTS, LwProgress *progress);
gboolean lw_cachefile_read (LwCacheFile *self, const gchar *EXPECTED_CHECKSUM, LwProgress *progress);

G_END_DECLS

#endif

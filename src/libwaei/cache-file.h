#ifndef LW_CACHE_FILE_INCLUDED
#define LW_CACHE_FILE_INCLUDED

#include <glib-object.h>
#include "mapped-file.h"
#include "progress.h"

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_CACHE_FILE lw_cache_file_get_type ()
G_DECLARE_FINAL_TYPE (LwCacheFile, lw_cache_file, LW, CACHE_FILE, LwMappedFile)

#define LW_CACHE_FILE_ERROR lw_cache_file_error_quark ()

typedef enum {
  LW_CACHE_FILE_ERRORCODE_INVALID_CHECKSUM,
  LW_CACHE_FILE_ERRORCODE_CORRUPT_CONTENTS
} LwCacheFileErrorCode;

//Methods
LwCacheFile * lw_cache_file_new (gchar const * PATH);
gsize lw_cache_file_write (LwCacheFile *self, const gchar *CHECKSUM, const gchar *CONTENTS, gssize content_length, LwProgress  *progress);
gsize lw_cache_file_write_cache_file (LwCacheFile  * self, const gchar * CHECKSUM, LwCacheFile * cache_file, LwProgress * progress);
gsize lw_cache_file_read (LwCacheFile *self, const gchar *EXPECTED_CHECKSUM, LwProgress *progress);

//Properties
const gchar* lw_cache_file_get_checksum (LwCacheFile *self);
gchar* lw_cache_file_get_contents (LwCacheFile *self);
gsize lw_cache_file_length (LwCacheFile *self);


G_END_DECLS

#endif

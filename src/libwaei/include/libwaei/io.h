#ifndef LW_IO_INCLUDED
#define LW_IO_INCLUDED

#include <stdio.h>

#include <glib.h>
#include <gio/gio.h>
#include <libwaei/progress.h>

G_BEGIN_DECLS

#define LW_IO_ERROR lw_io_error_quark ()

typedef enum {
  LW_IO_ERRORCODE_WRITE_ERROR,
  LW_IO_ERRORCODE_CORRUPT_ARCHIVE,
  LW_IO_ERRORCODE_TEXT_ENCODING_CONVERSION_ERROR,
  LW_IO_ERRORCODE_DOWNLOAD_ERROR,
  LW_IO_ERRORCODE_FAILED_REMOVING_FILE
} LwIoErrorCode;

struct _LwIoWriteChunkData {
  FILE *stream;
  gsize bytes_written;
};
typedef struct _LwIoWriteChunkData LwIoWriteChunkData;

gsize lw_io_fwrite (FILE *stream, const gchar *TEXT, gint length, LwProgress  *progress);
gsize lw_io_write_file (const gchar *PATH, const gchar *MODE, const gchar *TEXT, gint length, LwProgress  *progress);
gsize lw_io_write_chunk (gchar *chunk, gsize chunk_length, LwIoWriteChunkData *data, GError **error);

gboolean lw_io_remove (const gchar *URI, LwProgress *progress);

gboolean lw_io_copy (const gchar *SOURCE_PATH, const gchar *TARGET_PATH, LwProgress *progress);
gboolean lw_io_copy_with_encoding (const gchar *SOURCE_PATH, const gchar *TARGET_PATH, const gchar *SOURCE_ENCODING, const gchar *TARGET_ENCODING, LwProgress *progress);

gboolean lw_io_download (const gchar*SOURCE_PATH, const gchar*TARGET_PATH, LwProgress *progress);
gboolean lw_io_gunzip_file (const gchar *SOURCE_PATH, const gchar *TARGET_PATH, LwProgress *progress);

gsize lw_io_get_pagesize (void);

gchar* lw_io_allocate_temporary_file (gsize bytes_length, LwProgress  *progress);

gsize lw_io_get_file_size (char const * PATH);

G_END_DECLS

#endif

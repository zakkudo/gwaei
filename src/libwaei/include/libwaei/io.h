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
  LW_IO_ERRORCODE_TEXT_ENCODING_CONVERSION_ERROR
} LwIoErrorCode;

void lw_io_fwrite (FILE *stream, const gchar *TEXT, gint length, LwProgress  *progress);
void lw_io_write_file (const gchar *PATH, const gchar *MODE, const gchar *TEXT, gint length, LwProgress  *progress);

gboolean lw_io_remove (const gchar *URI, LwProgress *progress);

gboolean lw_io_copy (const gchar *SOURCE_PATH, const gchar *TARGET_PATH, LwProgress *progress);
gboolean lw_io_copy_with_encoding (const gchar *SOURCE_PATH, const gchar *TARGET_PATH, const gchar *SOURCE_ENCODING, const gchar *TARGET_ENCODING, LwProgress *progress);

gboolean lw_io_download (const gchar*SOURCE_PATH, const gchar*TARGET_PATH, LwProgress *progress);
gboolean lw_io_gunzip_file (const gchar *SOURCE_PATH, const gchar *TARGET_PATH, LwProgress *progress);

gsize lw_io_get_pagesize (void);

void lw_io_allocate_file (FILE *stream, gsize byte_length, LwProgress *progress);

G_END_DECLS

#endif

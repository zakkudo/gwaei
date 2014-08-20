#ifndef LW_IO_INCLUDED
#define LW_IO_INCLUDED

#include <stdio.h>

#include <glib.h>
#include <gio/gio.h>
#include <libwaei/progress.h>

G_BEGIN_DECLS

#define LW_IO_WRITE_ERROR lw_io_write_error_quark ()
#define LW_IO_READ_ERROR lw_io_read_error_quark ()

void lw_io_fwrite (FILE *stream, const gchar *TEXT, gint length, LwProgress  *progress);
void lw_io_write_file (const gchar *PATH, const gchar *MODE, const gchar *TEXT, gint length, LwProgress  *progress);

gboolean lw_io_remove (const gchar *URI, LwProgress *progress);

G_END_DECLS

#endif

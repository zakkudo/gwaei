#ifndef LW_FILE_INCLUDED
#define LW_FILE_INCLUDED

G_BEGIN_DECLS


typedef struct _LwFile LwFile;

LwFile* lw_file_new (const gchar *PATH);
void lw_file_free (LwFile *self);

const gchar* lw_file_get_path (LwFile *file);
const gchar* lw_file_get_basename (LwFile *file);
const gchar* lw_file_get_suffixless (LwFile *file);
const gchar* lw_file_get_suffix (LwFile *file);


G_END_DECLS

#endif

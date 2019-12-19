#ifndef LW_FILE_PATH_INCLUDED
#define LW_FILE_PATH_INCLUDED

G_BEGIN_DECLS


typedef struct _LwFilePath LwFilePath;

#define LW_FILE_PATH(obj) (LwFilePath*)obj

LwFilePath* lw_file_path_new (const gchar *PATH);
void lw_file_path_free (LwFilePath *self);

const gchar* lw_file_path_get_path (LwFilePath *self);
const gchar* lw_file_path_get_basename (LwFilePath *self);
const gchar* lw_file_path_get_suffixless (LwFilePath *self);
const gchar* lw_file_path_get_suffix (LwFilePath *self);


G_END_DECLS

#endif

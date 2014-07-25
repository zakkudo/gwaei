#ifndef LW_FILEPATH_INCLUDED
#define LW_FILEPATH_INCLUDED

G_BEGIN_DECLS


typedef struct _LwFilePath LwFilePath;

#define LW_FILEPATH(obj) (LwFilePath*)obj

LwFilePath* lw_filepath_new (const gchar *PATH);
void lw_filepath_free (LwFilePath *self);

const gchar* lw_filepath_get_path (LwFilePath *self);
const gchar* lw_filepath_get_basename (LwFilePath *self);
const gchar* lw_filepath_get_suffixless (LwFilePath *self);
const gchar* lw_filepath_get_suffix (LwFilePath *self);


G_END_DECLS

#endif

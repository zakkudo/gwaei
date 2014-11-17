#ifndef LW_MAPPEDFILE_INCLUDED
#define LW_MAPPEDFILE_INCLUDED 

G_BEGIN_DECLS

typedef struct _LwMappedFile LwMappedFile;

LwMappedFile* lw_mappedfile_new (gchar const * PATH, gboolean writable,	GError **error);
void lw_mappedfile_free (LwMappedFile *self, gboolean free_path);
void lw_mappedfile_unref (LwMappedFile *self);
LwMappedFile* lw_mappedfile_ref (LwMappedFile *self);
GType lw_mappedfile_get_type (void);

gsize lw_mappedfile_get_length (LwMappedFile *self);
gchar* lw_mappedfile_get_contents (LwMappedFile *self);
gchar const* lw_mappedfile_get_path (LwMappedFile *self);

G_END_DECLS

#endif

#ifndef LW_MAPPED_FILE_INCLUDED
#define LW_MAPPED_FILE_INCLUDED

G_BEGIN_DECLS

//Boilerplate
#define LW_TYPE_MAPPED_FILE lw_mapped_file_get_type ()
G_DECLARE_DERIVABLE_TYPE (LwMappedFile, lw_mapped_file, LW, MAPPED_FILE, GObject)

struct _LwMappedFileClass {
  GObjectClass parent_class;
};

//Methods
LwMappedFile* lw_mapped_file_new (gchar const * PATH);
GType lw_mapped_file_get_type (void) G_GNUC_CONST;

//Properties
gsize lw_mapped_file_length (LwMappedFile * self);
gchar * lw_mapped_file_get_contents (LwMappedFile  * self);

void lw_mapped_file_set_path (LwMappedFile * self, gchar const * PATH);
gchar const * lw_mapped_file_get_path (LwMappedFile * self);

void lw_mapped_file_set_delete_on_free (LwMappedFile * self, gboolean delete_on_free);
gboolean lw_mapped_file_get_delete_on_free (LwMappedFile * self);

void lw_mapped_file_set_writable (LwMappedFile *self, gboolean writable);
gboolean lw_mapped_file_is_writable (LwMappedFile *self);


#endif

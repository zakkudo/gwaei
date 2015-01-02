#ifndef LW_MAPPEDFILE_INCLUDED
#define LW_MAPPEDFILE_INCLUDED

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwMappedFile LwMappedFile;
typedef struct _LwMappedFileClass LwMappedFileClass;
typedef struct _LwMappedFilePrivate LwMappedFilePrivate;
typedef struct _LwMappedFileClassPrivate LwMappedFileClassPrivate;

#define LW_TYPE_MAPPEDFILE              (lw_mappedfile_get_type())
#define LW_MAPPEDFILE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_MAPPEDFILE, LwMappedFile))
#define LW_MAPPEDFILE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_MAPPEDFILE, LwMappedFileClass))
#define LW_IS_MAPPEDFILE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_MAPPEDFILE))
#define LW_IS_MAPPEDFILE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_MAPPEDFILE))
#define LW_MAPPEDFILE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_MAPPEDFILE, LwMappedFileClass))

struct _LwMappedFile {
  GObject object;
  LwMappedFilePrivate *priv;
};

struct _LwMappedFileClass {
  GObjectClass parent_class;
  LwMappedFileClassPrivate *priv;
};


//Methods
LwMappedFile* lw_mappedfile_new (gchar const * PATH);
GType lw_mappedfile_get_type (void) G_GNUC_CONST;

//Properties
gsize lw_mappedfile_length (LwMappedFile * self);
gchar * lw_mappedfile_get_contents (LwMappedFile  * self);

void lw_mappedfile_set_path (LwMappedFile * self, gchar const * PATH);
gchar const * lw_mappedfile_get_path (LwMappedFile * self);

void lw_mappedfile_set_delete_on_free (LwMappedFile * self, gboolean delete_on_free);
gboolean lw_mappedfile_get_delete_on_free (LwMappedFile * self);

gboolean lw_mappedfile_get_writable (LwMappedFile *self);


#endif

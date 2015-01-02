#ifndef LW_MAPPEDFILE_PRIVATE_INCLUDED
#define LW_MAPPEDFILE_PRIVATE_INCLUDED

G_BEGIN_DECLS

typedef enum
{
  PROP_0,
  PROP_CONTENTS,
  PROP_CONTENT_LENGTH,
  PROP_DELETE_ON_FREE,
  PROP_WRITABLE,
  PROP_PATH,
  TOTAL_PROPS
} Prop;

typedef enum {
  CLASS_SIGNALID_UNUSED,
  TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

struct _LwMappedFilePrivate {
	gchar * path;
  GMappedFile * mapped_file;
  gboolean delete_on_free;
  gboolean writable;
};

struct _LwMappedFileClassPrivate {
  guint signalid[TOTAL_CLASS_SIGNALIDS];
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_MAPPEDFILE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_MAPPEDFILE, LwMappedFilePrivate));

//Properties
static void lw_mappedfile_set_writable (LwMappedFile *self, gboolean writable);

G_END_DECLS

#endif


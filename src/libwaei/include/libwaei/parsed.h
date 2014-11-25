#ifndef LW_PARSED_INCLUDED
#define LW_PARSED_INCLUDED 

#include "dictionaryline.h"
#include "cachefile.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwParsed LwParsed;
typedef struct _LwParsedClass LwParsedClass;
typedef struct _LwParsedPrivate LwParsedPrivate;

#define LW_TYPE_PARSED              (lw_parsed_get_type())
#define LW_PARSED(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_PARSED, LwParsed))
#define LW_PARSED_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_PARSED, LwParsedClass))
#define LW_IS_PARSED(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_PARSED))
#define LW_IS_PARSED_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_PARSED))
#define LW_PARSED_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_PARSED, LwParsedClass))

struct _LwParsed {
  LwSerializable object;
  LwParsedPrivate *priv;
};

struct _LwParsedClass {
  LwSerializableClass parent_class;
};


typedef gboolean (*LwParsedForeachFunc) (LwParsed *self, LwDictionaryLine *line, gpointer user_data);

LwParsed* lw_parsed_new (gchar *contents, gsize content_length);

GType lw_parsed_get_type (void) G_GNUC_CONST;

void lw_parsed_foreach (LwParsed *self, LwParsedForeachFunc func, gpointer data);
LwDictionaryLine* lw_parsed_get_line (LwParsed *self, gsize line_number);
gsize lw_parsed_length (LwParsed *self);

gsize lw_parsed_serialize (LwParsed *self, gchar *preallocated_buffer, LwProgress *progress);
gsize lw_parsed_deserialize_into (LwParsed *self, const gchar *serialized_data, gsize serialized_length, LwProgress *progress);

void lw_parsed_set_lines (LwParsed *self, LwDictionaryLine *lines, gsize num_lines);

G_END_DECLS

#endif


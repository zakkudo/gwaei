#ifndef LW_PARSED_INCLUDED
#define LW_PARSED_INCLUDED 

#include "serializable.h"
#include "parsedline.h"
#include "cache-file.h"

G_BEGIN_DECLS

#define LW_PARSED_ERROR lw_parsed_error_quark ()
GQuark lw_parsed_error_quark (void);

//Boilerplate
typedef struct _LwParsed LwParsed;
typedef struct _LwParsedClass LwParsedClass;
typedef struct _LwParsedPrivate LwParsedPrivate;
typedef struct _LwParsedClassPrivate LwParsedClassPrivate;

#define LW_TYPE_PARSED              (lw_parsed_get_type())
#define LW_PARSED(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_PARSED, LwParsed))
#define LW_PARSED_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_PARSED, LwParsedClass))
#define LW_IS_PARSED(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_PARSED))
#define LW_IS_PARSED_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_PARSED))
#define LW_PARSED_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_PARSED, LwParsedClass))

struct _LwParsed {
  LwSerializable object;
};

struct _LwParsedClass {
  LwSerializableClass parent_class;
  LwParsedClassPrivate *priv;
};

typedef enum {
  LW_PARSED_ERRORCODE_DESERIALIZATION_ERROR,
  TOTAL_LW_PARSED_ERRORCODES,
} LwParsedErrorCode;

typedef gboolean (*LwParsedForeachFunc) (LwParsed *self, LwParsedLine *line, gpointer user_data);

LwParsed* lw_parsed_new (LwCacheFile * cache_file);

GType lw_parsed_get_type (void) G_GNUC_CONST;

void lw_parsed_foreach (LwParsed *self, LwParsedForeachFunc func, gpointer data);
LwParsedLine* lw_parsed_get_line (LwParsed *self, gsize line_number);

void lw_parsed_set_lines (LwParsed *self, LwParsedLine *lines, gsize num_lines);
LwParsedLine * lw_parsed_get_lines (LwParsed * self, gsize * num_lines_out);

gsize lw_parsed_num_lines (LwParsed * self);

LwCacheFile * lw_parsed_get_cachefile (LwParsed * self);

G_END_DECLS

#endif


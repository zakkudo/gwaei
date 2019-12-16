#ifndef LW_DICTIONARYMODULE_PRIVATE_INCLUDED
#define LW_DICTIONARYMODULE_PRIVATE_INCLUDED

#include <gmodule.h>

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_PATH,
  PROP_NAME,
  TOTAL_PROPS
} Props;


struct _LwDictionaryModulePrivate {
  gchar * path;
  gchar * name;
  GModule *module;
};

struct _LwDictionaryModuleReader {
  gchar ** paths;
  GDir * dir;
  gint i;
};

typedef GType (*LwDictionaryModuleRegisterTypeFunc)(GTypeModule * self);

struct _LwDictionaryModuleClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

//Properties

static gboolean lw_dictionarymodule_load (GTypeModule *module);
static void lw_dictionarymodule_unload (GTypeModule *module);

G_END_DECLS

#endif






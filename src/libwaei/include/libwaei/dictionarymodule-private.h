#ifndef LW_DICTIONARYMODULE_PRIVATE_INCLUDED
#define LW_DICTIONARYMODULE_PRIVATE_INCLUDED

#include <gmodule.h>

G_BEGIN_DECLS

typedef enum {
  PROP_0,
  PROP_NAME,
  TOTAL_PROPS
} Props;


struct _LwDictionaryModulePrivate {
  gchar *name;
  GModule *module;
};

struct _LwDictionaryModuleClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

#define LW_DICTIONARYMODULE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), LW_TYPE_DICTIONARYMODULE, LwDictionaryModulePrivate));

//Properties

static void lw_dictionarymodule_set_name (LwDictionaryModule * self, gchar const * NAME);

static gboolean lw_dictionarymodule_load (GTypeModule *module);
static void lw_dictionarymodule_unload (GTypeModule *module);

G_END_DECLS

#endif






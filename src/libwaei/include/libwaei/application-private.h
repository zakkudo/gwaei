#ifndef LW_APPLICATION_PRIVATE_INCLUDED
#define LW_APPLICATION_PRIVATE_INCLUDED

G_BEGIN_DECLS

#include "dictionarymodule.h"

typedef enum {
  PROP_0,
  TOTAL_PROPS
} Props;

struct _LwApplicationPrivate {
  GTree *dictionarymodules;
};

struct _LwApplicationClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
};

//Methods

static void lw_application_add_dictionarymodule (LwApplication * self, LwDictionaryModule * dictionarymodule);
static void lw_application_load_dictionarymodules (LwApplication * self);

G_END_DECLS

#endif

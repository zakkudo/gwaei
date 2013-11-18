#ifndef LW_MORPHOLOGYENGINE_INCLUDED
#define LW_MORPHOLOGYENGINE_INCLUDED 

#include <glib-object.h>
#ifdef HAVE_MECAB
#include <mecab.h>
#endif
#ifdef HAVE_HUNSPELL
#include <hunspell/hunspell.h>
#endif

#include "morphologyindex.h"
#include "morphologylist.h"

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwMorphologyEngine LwMorphologyEngine;
typedef struct _LwMorphologyEngineClass LwMorphologyEngineClass;
typedef struct _LwMorphologyEnginePrivate LwMorphologyEnginePrivate;

#define LW_TYPE_MORPHOLOGYENGINE              (lw_morphologyengine_get_type())
#define LW_MORPHOLOGYENGINE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_MORPHOLOGYENGINE, LwMorphologyEngine))
#define LW_MORPHOLOGYENGINE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_MORPHOLOGYENGINE, LwMorphologyEngineClass))
#define LW_IS_MORPHOLOGYENGINE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_MORPHOLOGYENGINE))
#define LW_IS_MORPHOLOGYENGINE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_MORPHOLOGYENGINE))
#define LW_MORPHOLOGYENGINE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_MORPHOLOGYENGINE, LwMorphologyEngineClass))

#define LW_MORPHOLOGY_SPELLCHECK_DELIMITOR ";"


struct _LwMorphologyEngine {
  GObject object;
  GMutex mutex;
#ifdef HAVE_MECAB
  mecab_t *mecab;
#endif
#ifdef HAVE_HUNSPELL
  Hunhandle *hunspell;
#endif
  gchar *locale;
};

struct _LwMorphologyEngineClass {
  GObjectClass parent_class;
};

//Methods
GType lw_morphologyengine_get_type (void) G_GNUC_CONST;

//Methods
LwMorphologyEngine* lw_morphologyengine_new (const gchar*);
LwMorphologyList* lw_morphologyengine_analyze (LwMorphologyEngine *engine, const gchar *TEXT, gboolean spellcheck);

#endif


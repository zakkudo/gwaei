#ifndef LW_DICTIONARYMODULE_INCLUDED
#define LW_DICTIONARYMODULE_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwDictionaryModule LwDictionaryModule;
typedef struct _LwDictionaryModuleClass LwDictionaryModuleClass;
typedef struct _LwDictionaryModulePrivate LwDictionaryModulePrivate;
typedef struct _LwDictionaryModuleClassPrivate LwDictionaryModuleClassPrivate;

#define LW_TYPE_DICTIONARYMODULE              (lw_dictionarymodule_get_type())
#define LW_DICTIONARYMODULE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARYMODULE, LwDictionaryModule))
#define LW_DICTIONARYMODULE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARYMODULE, LwDictionaryModuleClass))
#define LW_IS_DICTIONARYMODULE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARYMODULE))
#define LW_IS_DICTIONARYMODULE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARYMODULE))
#define LW_DICTIONARYMODULE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARYMODULE, LwDictionaryModuleClass))

struct _LwDictionaryModule {
  GTypeModule object;
  LwDictionaryModulePrivate *priv;
};

struct _LwDictionaryModuleClass {
  GTypeModuleClass parent_class;
  LwDictionaryModuleClassPrivate *priv;
};


typedef struct _LwDictionaryModuleReader LwDictionaryModuleReader;

LwDictionaryModuleReader * lw_dictionarymodulereader_open (gchar const * SEARCHPATH_OVERRIDE);
void lw_dictionarymodulereader_close (LwDictionaryModuleReader * self);
gchar * lw_dictionarymodulereader_read_path (LwDictionaryModuleReader * self);

//Methods
GTypeModule* lw_dictionarymodule_new (const gchar *PATH);
GType lw_dictionarymodule_get_type (void) G_GNUC_CONST;

void lw_dictionarymodule_set_path (LwDictionaryModule * self, gchar const * PATH);
gchar const * lw_dictionarymodule_get_path (LwDictionaryModule * self);

gchar const * lw_dictionarymodule_get_name (LwDictionaryModule * self);
gchar ** lw_dictionarymodule_get_dictionarylib_searchpath (gchar const * SEARCHPATH_OVERRIDE);

G_END_DECLS

#endif

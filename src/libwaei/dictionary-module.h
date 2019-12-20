#ifndef LW_DICTIONARY_MODULE_INCLUDED
#define LW_DICTIONARY_MODULE_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwDictionaryModule LwDictionaryModule;
typedef struct _LwDictionaryModuleClass LwDictionaryModuleClass;
typedef struct _LwDictionaryModulePrivate LwDictionaryModulePrivate;
typedef struct _LwDictionaryModuleClassPrivate LwDictionaryModuleClassPrivate;

#define LW_TYPE_DICTIONARY_MODULE              (lw_dictionary_module_get_type())
#define LW_DICTIONARY_MODULE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARY_MODULE, LwDictionaryModule))
#define LW_DICTIONARY_MODULE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARY_MODULE, LwDictionaryModuleClass))
#define LW_IS_DICTIONARY_MODULE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARY_MODULE))
#define LW_IS_DICTIONARY_MODULE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARY_MODULE))
#define LW_DICTIONARY_MODULE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARY_MODULE, LwDictionaryModuleClass))

struct _LwDictionaryModule {
  GTypeModule object;
};

struct _LwDictionaryModuleClass {
  GTypeModuleClass parent_class;
  LwDictionaryModuleClassPrivate *priv;
};


typedef struct _LwDictionaryModuleReader LwDictionaryModuleReader;

LwDictionaryModuleReader * lw_dictionary_modulereader_open (gchar const * SEARCHPATH_OVERRIDE);
void lw_dictionary_modulereader_close (LwDictionaryModuleReader * self);
gchar * lw_dictionary_modulereader_read_path (LwDictionaryModuleReader * self);

//Methods
GTypeModule* lw_dictionary_module_new (const gchar *PATH);
GType lw_dictionary_module_get_type (void) G_GNUC_CONST;

void lw_dictionary_module_set_path (LwDictionaryModule * self, gchar const * PATH);
gchar const * lw_dictionary_module_get_path (LwDictionaryModule * self);

gchar const * lw_dictionary_module_get_name (LwDictionaryModule * self);
gchar ** lw_dictionary_module_get_dictionarylib_searchpath (gchar const * SEARCHPATH_OVERRIDE);

gboolean lw_dictionary_module_symbol (LwDictionaryModule * self, gchar const * SYMBOL_NAME, gpointer * symbol);

G_END_DECLS

#endif

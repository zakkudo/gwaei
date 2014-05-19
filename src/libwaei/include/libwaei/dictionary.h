#ifndef LW_DICTIONARY_INCLUDED
#define LW_DICTIONARY_INCLUDED

#include <libwaei/result.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwDictionary LwDictionary;
typedef struct _LwDictionaryClass LwDictionaryClass;
typedef struct _LwDictionaryPrivate LwDictionaryPrivate;
typedef struct _LwDictionaryClassPrivate LwDictionaryClassPrivate;
typedef struct _LwDictionaryInstall LwDictionaryInstall;

#define LW_TYPE_DICTIONARY              (lw_dictionary_get_type())
#define LW_DICTIONARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_DICTIONARY, LwDictionary))
#define LW_DICTIONARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_DICTIONARY, LwDictionaryClass))
#define LW_IS_DICTIONARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_DICTIONARY))
#define LW_IS_DICTIONARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_DICTIONARY))
#define LW_DICTIONARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_DICTIONARY, LwDictionaryClass))


struct _LwDictionary {
  GObject object;
  LwDictionaryPrivate *priv;
};

struct _LwDictionaryClass {
  GObjectClass parent_class;
  LwDictionaryClassPrivate *priv;
};

//Methods
GType lw_dictionary_get_type (void) G_GNUC_CONST;

gchar* lw_dictionary_get_path (LwDictionary *self);

void lw_dictionary_set_filename (LwDictionary *self, const gchar *FILENAME);
const gchar* lw_dictionary_get_filename (LwDictionary *self);

const gchar* lw_dictionary_get_name (LwDictionary *self);

LwMorphologyEngine* lw_dictionary_get_morphologyengine (LwDictionary *self);
void lw_dictionary_set_morphologyengine (LwDictionary *self, LwMorphologyEngine *morphology_engine);

LwProgress* lw_dictionary_get_progress (LwDictionary *self);
void lw_dictionary_set_progress (LwDictionary *self, LwProgress *progress);

size_t lw_dictionary_length (LwDictionary *self);
const gchar* lw_dictionary_get_buffer (LwDictionary *self);

gboolean lw_dictionary_equals (LwDictionary *dictionary1, LwDictionary *dictionary2);

gchar** lw_dictionary_get_installed_idlist (GType type_filter);
gchar* lw_dictionary_get_directoryname (GType dictionary_type);
gchar* lw_dictionary_build_id (LwDictionary *self);
gchar* lw_dictionary_build_id_from_type (GType type, const gchar *FILENAME);
gchar* lw_dictionary_directoryname_to_typename (const gchar *DIRECTORYNAME);

gboolean  lw_dictionary_uninstall (LwDictionary *self);

G_END_DECLS

#include <libwaei/dictionary-index.h>
#include <libwaei/dictionary-callbacks.h>

#endif

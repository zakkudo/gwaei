#ifndef W_APPLICATION_INCLUDED
#define W_APPLICATION_INCLUDED

#include <libwaei/application.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _WApplication WApplication;
typedef struct _WApplicationClass WApplicationClass;
typedef struct _WApplicationPrivate WApplicationPrivate;
typedef struct _WApplicationClassPrivate WApplicationClassPrivate;

#define W_TYPE_APPLICATION              (w_application_get_type())
#define W_APPLICATION(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), W_TYPE_APPLICATION, WApplication))
#define W_APPLICATION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), W_TYPE_APPLICATION, WApplicationClass))
#define W_IS_APPLICATION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), W_TYPE_APPLICATION))
#define W_IS_APPLICATION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), W_TYPE_APPLICATION))
#define W_APPLICATION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), W_TYPE_APPLICATION, WApplicationClass))

struct _WApplication {
  LwApplication application;
};

struct _WApplicationClass {
  LwApplicationClass parent_class;
  WApplicationClassPrivate *priv;
};

//Methods
GApplication*  w_application_new (GApplicationFlags flags);
GType w_application_get_type (void) G_GNUC_CONST;

const char* w_application_get_program_name (WApplication* application);

LwDictionary* w_application_fuzzy_find_dictionary (WApplication *self, const gchar *FUZZY_DESCRIPTION);
LwDictionaryInstall* w_application_fuzzy_find_dictionaryinstall (WApplication *self, const gchar *FUZZY_DESCRIPTION);

//Properties

LwDictionaryList* w_application_get_dictionarylist (WApplication* application);
LwDictionaryInstallList* w_application_get_dictionaryinstalllist (WApplication* application);

LwPreferences* w_application_get_preferences (WApplication *application);
void w_application_set_preferences (WApplication *application, LwPreferences *preferences);

G_END_DECLS

#endif

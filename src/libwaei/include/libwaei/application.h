#ifndef LW_APPLICATION_INCLUDED
#define LW_APPLICATION_INCLUDED

#include <gio/gio.h>

G_BEGIN_DECLS

//Boilerplate
typedef struct _LwApplication LwApplication;
typedef struct _LwApplicationClass LwApplicationClass;
typedef struct _LwApplicationPrivate LwApplicationPrivate;
typedef struct _LwApplicationClassPrivate LwApplicationClassPrivate;

#define LW_TYPE_APPLICATION              (lw_application_get_type())
#define LW_APPLICATION(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_APPLICATION, LwApplication))
#define LW_APPLICATION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_APPLICATION, LwApplicationClass))
#define LW_IS_APPLICATION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_APPLICATION))
#define LW_IS_APPLICATION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_APPLICATION))
#define LW_APPLICATION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_APPLICATION, LwApplicationClass))

struct _LwApplication {
  GApplication application;
  LwApplicationPrivate *priv;
};

struct _LwApplicationClass {
  GApplicationClass parent_class;
  LwApplicationClassPrivate *priv;
};

//Methods
GApplication*  lw_application_new (GApplicationFlags flags);
GType lw_application_get_type (void) G_GNUC_CONST;

char const * lw_application_get_program_name (LwApplication* application);

//Properties


G_END_DECLS

#endif

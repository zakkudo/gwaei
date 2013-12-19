#ifndef LGW_WINDOW_INCLUDED
#define LGW_WINDOW_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LgwWindow LgwWindow;
typedef struct _LgwWindowClass LgwWindowClass;
typedef struct _LgwWindowPrivate LgwWindowPrivate;

#define LGW_TYPE_WINDOW              (lgw_window_get_type())
#define LGW_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_WINDOW, LgwWindow))
#define LGW_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_WINDOW, LgwWindowClass))
#define LGW_IS_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_WINDOW))
#define LGW_IS_WINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_WINDOW))
#define LGW_WINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_WINDOW, LgwWindowClass))

struct _LgwWindow {
  GtkApplicationWindow window;
  LgwWindowPrivate *priv;
};

struct _LgwWindowClass {
  GtkApplicationWindowClass parent_class;
};

//Methods
GtkWindow* lgw_window_new (void);
GType lgw_window_get_type (void) G_GNUC_CONST;

G_END_DECLS

#include "window-callbacks.h"

#endif

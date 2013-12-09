#ifndef LGW_STACKWIDGET_INCLUDED
#define LGW_STACKWIDGET_INCLUDED

#include <glib-object.h>

#define LGW_TYPE_STACKWIDGET                (lgw_stackwidget_get_type ())
#define LGW_STACKWIDGET(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), LGW_TYPE_STACKWIDGET, LgwStackWidget))
#define LGW_IS_STACKWIDGET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LGW_TYPE_STACKWIDGET))
#define LGW_STACKWIDGET_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), LGW_TYPE_STACKWIDGET, LgwStackWidgetInterface))

typedef struct _LgwStackWidget LgwStackWidget; /* dummy object */
typedef struct _LgwStackWidgetInterface LgwStackWidgetInterface;

struct _LgwStackWidgetInterface {
      GTypeInterface parent;
      GMenuModel* (*get_button_menu_model) (LgwStackWidget *self);
      GMenuModel* (*get_window_menu_model) (LgwStackWidget *self);
      GList* (*get_actions) (LgwStackWidget *self);
};

GType lgw_stackwidget_get_type (void);

GMenuModel* lgw_stackwidget_get_button_menu_model (LgwStackWidget *self);
GMenuModel* lgw_stackwidget_get_window_menu_model (LgwStackWidget *self);

GList* lgw_stackwidget_get_actions (LgwStackWidget *self);


#endif

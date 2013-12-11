#ifndef LGW_MENUABLE_INCLUDED
#define LGW_MENUABLE_INCLUDED

#include <glib-object.h>

#define LGW_TYPE_MENUABLE                (lgw_menuable_get_type ())
#define LGW_MENUABLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), LGW_TYPE_MENUABLE, LgwMenuable))
#define LGW_IS_MENUABLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LGW_TYPE_MENUABLE))
#define LGW_MENUABLE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), LGW_TYPE_MENUABLE, LgwMenuableInterface))

typedef struct _LgwMenuable LgwMenuable; /* dummy object */
typedef struct _LgwMenuableInterface LgwMenuableInterface;

struct _LgwMenuableInterface {
      GTypeInterface parent;
      GMenuModel* (*get_button_menu_model) (LgwMenuable *self);
      GMenuModel* (*get_window_menu_model) (LgwMenuable *self);
      GList* (*get_actions) (LgwMenuable *self);
};

GType lgw_menuable_get_type (void);

GMenuModel* lgw_menuable_get_button_menu_model (LgwMenuable *self);
GMenuModel* lgw_menuable_get_window_menu_model (LgwMenuable *self);


#endif

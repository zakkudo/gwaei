#ifndef LGW_ACTIONABLE_INCLUDED
#define LGW_ACTIONABLE_INCLUDED

G_BEGIN_DECLS

#include <glib-object.h>

#define LGW_TYPE_ACTIONABLE                (lgw_actionable_get_type ())
#define LGW_ACTIONABLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), LGW_TYPE_ACTIONABLE, LgwActionable))
#define LGW_IS_ACTIONABLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LGW_TYPE_ACTIONABLE))
#define LGW_ACTIONABLE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), LGW_TYPE_ACTIONABLE, LgwActionableInterface))

typedef struct _LgwActionable LgwActionable; /* dummy object */
typedef struct _LgwActionableInterface LgwActionableInterface;

struct _LgwActionableInterface {
      GTypeInterface parent;
      GList* (*get_actions) (LgwActionable *self);
      void (*sync_actions) (LgwActionable *self);
};

GType lgw_actionable_get_type (void);

GList* lgw_actionable_get_actions (LgwActionable *self);
void lgw_actionable_sync_actions (LgwActionable *self);


G_END_DECLS
    
#endif


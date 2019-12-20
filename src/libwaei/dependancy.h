#ifndef LW_DEPENDANCY_INCLUDED
#define LW_DEPENDANCY_INCLUDED

G_BEGIN_DECLS

typedef struct _LwDependancy LwDependancy;

#define LW_DEPENDANCY(obj) (LwDependancy*)obj
#define LW_TYPE_DEPENDANCY (lw_dependancy_get_type ())

LwDependancy* lw_dependancy_new (const gchar *NAME);
GType lw_dependancy_get_type (void);
LwDependancy* lw_dependancy_copy (LwDependancy* self);
void lw_dependancy_free (LwDependancy *self);

const gchar* lw_dependancy_get_name (LwDependancy *self);
void lw_dependancy_set_conditions (LwDependancy *self, GList* conditions);
gboolean lw_dependancy_is_required_for (LwDependancy *self, GObject *object);
void lw_dependancy_satisfy (LwDependancy *self, GObject *object);
gboolean lw_dependancy_is_satisfied_for (LwDependancy *self, GObject *object);
GObject* lw_dependancy_get_satisfaction (LwDependancy *self);

G_END_DECLS

#endif

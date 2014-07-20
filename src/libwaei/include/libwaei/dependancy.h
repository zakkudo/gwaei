#ifndef LW_DEPENDANCY_INCLUDED
#define LW_DEPENDANCY_INCLUDED

G_BEGIN_DECLS


struct _LwDependancy {
  GList *conditions;
  gchar *name;
  GObject *object;
};

typedef struct _LwDependancy LwDependancy;

#define LW_DEPENDANCY(obj) (LwDependancy*)obj

LwDependancy* lw_dependancy_new (const gchar *NAME);
LwDependancy* lw_dependancy_copy (LwDependancy* self);
void lw_dependancy_set_conditions (LwDependancy *self, GList* conditions);
gboolean lw_dependancy_is_required_for (LwDependancy *self, GObject *object);
void lw_dependancy_satisfy (LwDependancy *self, GObject *object);
gboolean lw_dependancy_is_satisfied (LwDependancy *self, GObject *object);
void lw_dependancy_free (LwDependancy *self);

G_END_DECLS

#endif

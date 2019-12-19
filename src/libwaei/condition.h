#ifndef LW_CONDITION_INCLUDED
#define LW_CONDITION_INCLUDED

G_BEGIN_DECLS

typedef struct _LwCondition LwCondition;

#define LW_CONDITION(obj) (LwCondition*)obj
#define LW_TYPE_CONDITION (lw_condition_get_type ())

LwCondition* lw_condition_new_boolean (const gchar *property_name, gboolean expected_value);
GType lw_condition_get_type (void);
LwCondition* lw_condition_copy (LwCondition *self);
void lw_condition_free (LwCondition *self);

gboolean lw_condition_matches (LwCondition *self, GObject *object);
gboolean lw_condition_list_matches (GList *conditions, GObject *object);


G_END_DECLS

#endif

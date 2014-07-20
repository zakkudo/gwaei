#ifndef LW_CONDITION_INCLUDED
#define LW_CONDITION_INCLUDED

G_BEGIN_DECLS

struct _LwCondition {
  gchar *property_name;
  GValue value;
};

typedef struct _LwCondition LwCondition;

#define LW_CONDITION(obj) (LwCondition*)obj

LwCondition* lw_condition_new_boolean (const gchar *property_name, gboolean expected_value);
void lw_condition_free (LwCondition *self);

gboolean lw_condition_matches (LwCondition *self, GObject *object);
gboolean lw_condition_list_matches (GList *conditions, GObject *object);

LwCondition* lw_condition_copy (LwCondition *self);

G_END_DECLS

#endif

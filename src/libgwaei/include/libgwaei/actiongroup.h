#ifndef LGW_ACTION_GROUP_INCLUDED
#define LGW_ACTION_GROUP_INCLUDED

G_BEGIN_DECLS

struct _LgwActionGroup {
 GActionEntry const * entries;
 gint length;
 GtkWidget *widget;
};

typedef struct _LgwActionGroup LgwActionGroup;

#define LGW_ACTIONGROUP(obj) ((LgwActionGroup*)(obj))

LgwActionGroup* lgw_actiongroup_static_new (GActionEntry* const entries, gint length, GtkWidget *widget);
void lgw_actiongroup_free (LgwActionGroup *group);

void lgw_actiongroup_add_to_map (LgwActionGroup *action_group, GActionMap *map);
void lgw_actiongroup_remove_from_map (LgwActionGroup *action_group, GActionMap *map);

gboolean lgw_actiongroup_contains_entries (LgwActionGroup *action_group, GActionEntry *other_entries, gint other_length);

G_END_DECLS

#endif


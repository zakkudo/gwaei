#ifndef LGW_LIBGWAEI_INCLUDED
#define LGW_LIBGWAEI_INCLUDED

#include <libwaei/libwaei.h>

#include <libgwaei/actiongroup.h>
#include <libgwaei/actionable.h>
#include <libgwaei/menuable.h>

#include <libgwaei/window.h>

#include <libgwaei/texttagtable.h>

#include <libgwaei/dictionaryliststore.h>
#include <libgwaei/dictionarylistbox.h>

#include <libgwaei/resultsview.h>
#include <libgwaei/resultstextview.h>
#include <libgwaei/searchentry.h>

#include <libgwaei/searchwidget.h>

#include <libgwaei/vocabularywordstore.h>
#include <libgwaei/vocabularywordview.h>

#include <libgwaei/vocabularyliststore.h>
#include <libgwaei/vocabularylistview.h>

#include <libgwaei/vocabularywidget.h>

gchar* lgw_get_symbolic_icon_name_if_exists (const gchar* ICON_NAME);
gboolean lgw_load_xml (GtkBuilder *builder, const gchar *FILENAME);
GMenuModel* lgw_load_menu_model (const gchar* FILENAME);

void lgw_menumodel_set_contents (GMenuModel *menu_model, GMenuModel *other_menu_model);

void lgw_application_add_accelerators (GtkApplication *application, GMenuModel *menumodel);
void lgw_application_remove_accelerators (GtkApplication *application, GMenuModel *menumodel);


#endif


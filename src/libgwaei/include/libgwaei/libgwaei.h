#ifndef LGW_LIBGWAEI_INCLUDED
#define LGW_LIBGWAEI_INCLUDED

#include <libwaei/libwaei.h>

#include <libgwaei/actiongroup.h>

#include <libgwaei/texttagtable.h>

#include <libgwaei/stackwidget.h>

#include <libgwaei/dictionarylist.h>
#include <libgwaei/dictionarylistbox.h>

#include <libgwaei/resultsview.h>
#include <libgwaei/resultstextview.h>
#include <libgwaei/searchentry.h>

#include <libgwaei/searchwidget.h>

/*
#include <libgwaei/vocabularylistview.h>
#include <libgwaei/vocabularywordview.h>

#include <libgwaei/vocabularywidget.h>
*/

gchar* lgw_get_symbolic_icon_name_if_exists (const gchar* ICON_NAME);
gboolean lgw_load_xml (GtkBuilder *builder, const gchar *FILENAME);
GMenuModel* lgw_load_menu_model (const gchar* FILENAME);


#endif


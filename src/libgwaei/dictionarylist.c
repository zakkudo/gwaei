/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Zachary Dovel. All Rights Reserved.

    LICENSE:
    This file is part of gWaei.

    gWaei is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gWaei is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with gWaei.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

//!
//! @file dictionarylist.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>
#include <libgwaei/dictionarylist-private.h>

static void lgw_dictionarylist_attach_signals (LgwDictionaryList*);

G_DEFINE_TYPE (LgwDictionaryList, lgw_dictionarylist, LW_TYPE_DICTIONARYLIST)

//!
//! @brief Sets up the dictionary manager.  This is the backbone of every portion of the GUI that allows editing dictionaries
//!
LgwDictionaryList* lgw_dictionarylist_new ()
{
    //Declarations
    LgwDictionaryList *dictionarylist;

    //Initializations
    dictionarylist = LGW_DICTIONARYLIST (g_object_new (LGW_TYPE_DICTIONARYLIST, NULL));

    return LGW_DICTIONARYLIST (dictionarylist);
}


void static
lgw_dictionarylist_init (LgwDictionaryList *dictionarylist)
{
    dictionarylist->priv = LGW_DICTIONARYLIST_GET_PRIVATE (dictionarylist);
    memset(dictionarylist->priv, 0, sizeof(LgwDictionaryListPrivate));

    LgwDictionaryListPrivate *priv;

    priv = dictionarylist->priv;

    GType types[] = { 
        G_TYPE_STRING,  //LGW_DICTIONARYLIST_COLUMN_IMAGE
        G_TYPE_STRING,  //LGW_DICTIONARYLIST_COLUMN_POSITION
        G_TYPE_STRING,  //LGW_DICTIONARYLIST_COLUMN_NAME
        G_TYPE_STRING,  //LGW_DICTIONARYLIST_COLUMN_LONG_NAME
        G_TYPE_STRING,  //LGW_DICTIONARYLIST_COLUMN_ENGINE
        G_TYPE_STRING,  //LGW_DICTIONARYLIST_COLUMN_SHORTCUT
        G_TYPE_BOOLEAN, //LGW_DICTIONARYLIST_COLUMN_SELECTED
        G_TYPE_POINTER  //LGW_DICTIONARYLIST_COLUMN_DICT_POINTER
    };

    priv->data.liststore = gtk_list_store_newv (TOTAL_LGW_DICTIONARYLIST_COLUMNS, types);
    priv->data.menumodel = G_MENU_MODEL (g_menu_new ());

    lgw_dictionarylist_attach_signals (dictionarylist);
}


static void 
lgw_dictionarylist_finalize (GObject *object)
{
    LgwDictionaryList *dictionarylist;
    LgwDictionaryListPrivate *priv;

    dictionarylist = LGW_DICTIONARYLIST (object);
    priv = dictionarylist->priv;

    if (priv->data.menumodel != NULL) g_object_unref (priv->data.menumodel); priv->data.menumodel = NULL;
    if (priv->data.liststore != NULL) g_object_unref (priv->data.liststore); priv->data.liststore = NULL;

    G_OBJECT_CLASS (lgw_dictionarylist_parent_class)->finalize (object);
}


static void
lgw_dictionarylist_class_init (LgwDictionaryListClass *klass)
{
    //Declarations
    GObjectClass *object_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lgw_dictionarylist_finalize;

    g_type_class_add_private (object_class, sizeof (LgwDictionaryListPrivate));
}


static void 
lgw_dictionarylist_attach_signals (LgwDictionaryList *dictionarylist)
{
    //Sanity checks
    g_return_if_fail (dictionarylist != NULL);    

    g_signal_connect (dictionarylist, "changed", G_CALLBACK (lgw_dictionarylist_changed_cb), NULL);
}


static void
lgw_dictionarylist_menumodel_append (LgwDictionaryList *dictionarylist, 
                                     LwDictionary     *dictionary)
{
    //Sanity checks
    g_return_if_fail (dictionarylist != NULL);
    g_return_if_fail (dictionary != NULL);

    //Declarations
    GMenuModel *menumodel;
    GMenuItem *menuitem;
    gint index;
    gchar *detailed_action;
    const gchar *shortname;
    gchar *longname;

    //Initializations
    menumodel = lgw_dictionarylist_get_menumodel (dictionarylist);
    index = g_menu_model_get_n_items (menumodel) + 1;
    longname = NULL;
    detailed_action = NULL;
    menuitem = NULL;
    shortname = lw_dictionary_get_name (dictionary);

    longname = g_strdup_printf (gettext("%s Dictionary"), shortname);
    if (longname == NULL) goto errored;
    detailed_action = g_strdup_printf ("win.set-dictionary::%d", index);
    if (detailed_action == NULL) goto errored;

    menuitem = g_menu_item_new (longname, detailed_action);
    if (menuitem == NULL) goto errored;
    g_menu_append_item (G_MENU (menumodel), menuitem);

errored:
    if (longname != NULL) g_free (longname); longname = NULL;
    if (detailed_action != NULL) g_free (detailed_action); detailed_action = NULL;
    if (menuitem != NULL) g_object_unref (menuitem); menuitem = NULL;
}


void
lgw_dictionarylist_sync_menumodel (LgwDictionaryList *dictionarylist)
{
    //Sanity checks
    g_return_if_fail (dictionarylist != NULL);

    //Declarations
    GMenuModel *menumodel;
    GMenu *menu;
    GList *link;
    LwDictionary *dictionary;

    //Initializations
    menumodel = lgw_dictionarylist_get_menumodel (dictionarylist);
    menu = G_MENU (menumodel);
    link = lw_dictionarylist_get_list (LW_DICTIONARYLIST (dictionarylist));

    while (g_menu_model_get_n_items (menumodel) > 0)
    {
      g_menu_remove (menu, 0);
    }

    while (link != NULL)
    {
      dictionary = LW_DICTIONARY (link->data);

      lgw_dictionarylist_menumodel_append (dictionarylist, dictionary);

      link = link->next;
    }
}


static void
lgw_dictionarylist_liststore_append (LgwDictionaryList *dictionarylist, LwDictionary *dictionary)
{
    //Sanity checks
    g_return_if_fail (dictionarylist != NULL);
    g_return_if_fail (dictionary != NULL);

    //Declarations
    GtkListStore *liststore;
    GtkTreeModel *treemodel;
    GtkTreeIter iter;
    gchar shortcutname[10];
    gchar ordernumber[10];
    const gchar *iconname;
    const static gchar *favoriteicon = "emblem-favorite";
    const gchar *shortname;
    gchar *longname;
    gchar *directoryname;
    gint index;
    gboolean selected;

    //Initializations
    liststore = lgw_dictionarylist_get_liststore (dictionarylist);
    treemodel = GTK_TREE_MODEL (liststore);
    index = gtk_tree_model_iter_n_children (treemodel, NULL) + 1;
    shortname = lw_dictionary_get_name (dictionary);
    iconname = NULL;
    *shortcutname = '\0';
    *ordernumber = '\0';
    longname = NULL;
    directoryname = NULL;

    longname = g_strdup_printf (gettext("%s Dictionary"), shortname);
    if (longname == NULL) goto errored;
    directoryname = lw_dictionary_get_directoryname (G_OBJECT_TYPE (dictionary));
    if (directoryname == NULL) goto errored;
    if (index == 1) iconname = favoriteicon;
    if (index < 10) sprintf (shortcutname, "Alt-%d", index);
    if (index < 1000) sprintf (ordernumber, "%d", index);
    selected = lw_dictionary_is_selected (dictionary);

    gtk_list_store_append (liststore, &iter);
    gtk_list_store_set (liststore, &iter,
        LGW_DICTIONARYLIST_COLUMN_IMAGE,        iconname,
        LGW_DICTIONARYLIST_COLUMN_POSITION,     ordernumber,
        LGW_DICTIONARYLIST_COLUMN_NAME,         shortname,
        LGW_DICTIONARYLIST_COLUMN_LONG_NAME,    longname,
        LGW_DICTIONARYLIST_COLUMN_ENGINE,       directoryname,
        LGW_DICTIONARYLIST_COLUMN_SHORTCUT,     shortcutname,
        LGW_DICTIONARYLIST_COLUMN_SELECTED,     selected,
        LGW_DICTIONARYLIST_COLUMN_DICT_POINTER, dictionary,
        -1
    );

errored:
    if (longname != NULL) g_free (longname); longname = NULL;
    if (directoryname != NULL) g_free (directoryname); directoryname = NULL;
}


void 
lgw_dictionarylist_sync_treestore (LgwDictionaryList *dictionarylist)
{
    //Sanity checks
    g_return_if_fail (dictionarylist != NULL);

    //Declarations
    GtkListStore *liststore;
    LwDictionary *dictionary;
    GList *link;

    liststore = lgw_dictionarylist_get_liststore (dictionarylist);
    link = lw_dictionarylist_get_list (LW_DICTIONARYLIST (dictionarylist));

    gtk_list_store_clear (liststore);

    while (link != NULL)
    {
      dictionary = LW_DICTIONARY (link->data);
      
      lgw_dictionarylist_liststore_append (dictionarylist, dictionary);

      link = link->next;
    }
}


static gint
lgw_dictionarylist_save_order_compare_func (gconstpointer a, gconstpointer b, gpointer data)
{
    //Declarations
    GHashTable *hashtable;
    gint a_index;
    gint b_index;

    //Initializations
    hashtable = data;
    a_index = GPOINTER_TO_INT (g_hash_table_lookup (hashtable, a));
    b_index = GPOINTER_TO_INT (g_hash_table_lookup (hashtable, b));
    
    if (a_index < b_index) return -1;
    else if (a_index > b_index) return 1;
    else return 0;
}


void
lgw_dictionarylist_save_order (LgwDictionaryList *dictionarylist, LwPreferences *preferences)
{
    //Declarations
    GtkListStore *liststore;
    GtkTreeModel *treemodel;
    GtkTreeIter treeiter;
    LwDictionary *dictionary;
    gboolean valid;
    GHashTable *hashtable;
    gint index;

    //Initializations
    liststore = lgw_dictionarylist_get_liststore (dictionarylist);
    treemodel = GTK_TREE_MODEL (liststore);
    hashtable = g_hash_table_new (g_direct_hash, g_direct_equal);
    index = 0;

    //Fill the hashtable
    valid = gtk_tree_model_get_iter_first (treemodel, &treeiter);
    while (valid)
    {
      gtk_tree_model_get (treemodel, &treeiter, LGW_DICTIONARYLIST_COLUMN_DICT_POINTER, &dictionary, -1);
      g_hash_table_insert (hashtable, dictionary, GINT_TO_POINTER (index));
      valid = gtk_tree_model_iter_next (treemodel, &treeiter);
      index++;
    }

    //Sort the LwDictionaryList and save the order
    lw_dictionarylist_sort_with_data (LW_DICTIONARYLIST (dictionarylist), lgw_dictionarylist_save_order_compare_func, hashtable);
    lw_dictionarylist_save_order (LW_DICTIONARYLIST (dictionarylist), preferences);

    g_hash_table_unref (hashtable); hashtable = NULL;

    lw_dictionarylist_load_order (LW_DICTIONARYLIST (dictionarylist), preferences);
}


GMenuModel*
lgw_dictionarylist_get_menumodel (LgwDictionaryList *dictionarylist)
{
    return dictionarylist->priv->data.menumodel;
}


GtkListStore*
lgw_dictionarylist_get_liststore (LgwDictionaryList *dictionarylist)
{
    return dictionarylist->priv->data.liststore;
}


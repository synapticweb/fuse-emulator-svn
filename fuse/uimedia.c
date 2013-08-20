/* uimedia.c: Disk media UI routines
   Copyright (c) 2013 Alex Badea

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: vamposdecampos@gmail.com

*/

#include <config.h>
#include <string.h>

#ifdef HAVE_LIB_GLIB
#include <glib.h>
#endif

#include "fuse.h"
#include "ui/ui.h"
#include "ui/uimedia.h"
#include "utils.h"

static GSList *registered_drives = NULL;

int
ui_media_drive_register( ui_media_drive_info_t *drive )
{
  registered_drives = g_slist_append( registered_drives, drive );
  return 0;
}

void
ui_media_drive_end( void )
{
  g_slist_free( registered_drives );
  registered_drives = NULL;
}

struct find_info {
  int controller;
  int drive;
};

static gint
find_drive( gconstpointer data, gconstpointer user_data )
{
  const ui_media_drive_info_t *drive = data;
  const struct find_info *info = user_data;

  return !( drive->controller_index == info->controller &&
            drive->drive_index == info->drive );
}

ui_media_drive_info_t *
ui_media_drive_find( int controller, int drive )
{
  struct find_info info = {
    /* .controller = */ controller,
    /* .drive = */ drive,
  };
  GSList *item;
  item = g_slist_find_custom( registered_drives, &info, find_drive );
  return item ? item->data : NULL;
}


static gint
any_available( gconstpointer data, gconstpointer user_data )
{
  const ui_media_drive_info_t *drive = data;

  return !( drive->is_available && drive->is_available() );
}

int
ui_media_drive_any_available( void )
{
  GSList *item;
  item = g_slist_find_custom( registered_drives, NULL, any_available );
  return item ? 1 : 0;
}


static void
update_parent_menus( gpointer data, gpointer user_data )
{
  const ui_media_drive_info_t *drive = data;

  if( drive->is_available && drive->menu_item_parent >= 0 )
    ui_menu_activate( drive->menu_item_parent, drive->is_available() );
}

void
ui_media_drive_update_parent_menus( void )
{
  g_slist_foreach( registered_drives, update_parent_menus, NULL );
}

static int
maybe_menu_activate( int id, int activate )
{
  if( id < 0 )
    return 0;
  return ui_menu_activate( id, activate );
}

void
ui_media_drive_update_menus( ui_media_drive_info_t *drive, unsigned flags )
{
  if( !drive->fdd )
    return;

  if( flags & UI_MEDIA_DRIVE_UPDATE_TOP )
    maybe_menu_activate( drive->menu_item_top, drive->fdd->type != FDD_TYPE_NONE );
  if( flags & UI_MEDIA_DRIVE_UPDATE_EJECT )
    maybe_menu_activate( drive->menu_item_eject, drive->fdd->loaded );
  if( flags & UI_MEDIA_DRIVE_UPDATE_FLIP )
    maybe_menu_activate( drive->menu_item_flip, !drive->fdd->upsidedown );
  if( flags & UI_MEDIA_DRIVE_UPDATE_WP )
    maybe_menu_activate( drive->menu_item_wp, !drive->fdd->wrprot );
}


int
ui_media_drive_flip( int controller, int which, int flip )
{
  ui_media_drive_info_t *drive;

  drive = ui_media_drive_find( controller, which );
  if( !drive )
    return -1;
  if( !drive->fdd->loaded )
    return 1;

  fdd_flip( drive->fdd, flip );
  ui_media_drive_update_menus( drive, UI_MEDIA_DRIVE_UPDATE_FLIP );
  return 0;
}

int
ui_media_drive_writeprotect( int controller, int which, int wrprot )
{
  ui_media_drive_info_t *drive;

  drive = ui_media_drive_find( controller, which );
  if( !drive )
    return -1;
  if( !drive->fdd->loaded )
    return 1;

  fdd_wrprot( drive->fdd, wrprot );
  ui_media_drive_update_menus( drive, UI_MEDIA_DRIVE_UPDATE_WP );
  return 0;
}

static int
drive_disk_write( ui_media_drive_info_t *drive, const char *filename )
{
  int error;

  drive->disk->type = DISK_TYPE_NONE;
  if( filename == NULL )
    filename = drive->disk->filename; /* write over original file */
  error = disk_write( drive->disk, filename );

  if( error != DISK_OK ) {
    ui_error( UI_ERROR_ERROR, "couldn't write '%s' file: %s", filename,
	      disk_strerror( error ) );
    return 1;
  }

  if( drive->disk->filename && strcmp( filename, drive->disk->filename ) ) {
    free( drive->disk->filename );
    drive->disk->filename = utils_safe_strdup( filename );
  }
  return 0;
}


int
ui_media_drive_save( int controller, int which, int saveas )
{
  ui_media_drive_info_t *drive;
  int err;
  char *filename = NULL, title[80];

  drive = ui_media_drive_find( controller, which );
  if( !drive )
    return -1;
  if( drive->disk->type == DISK_TYPE_NONE )
    return 0;
  if( drive->disk->filename == NULL )
    saveas = 1;

  fuse_emulation_pause();

  snprintf( title, sizeof( title ), "Fuse - Write %s", drive->name );
  if( saveas ) {
    filename = ui_get_save_filename( title );
    if( !filename ) {
      fuse_emulation_unpause();
      return 1;
    }
  }

  err = drive_disk_write( drive, filename );

  if( saveas )
    libspectrum_free( filename );

  fuse_emulation_unpause();
  if( err )
    return 1;

  drive->disk->dirty = 0;
  return 0;
}

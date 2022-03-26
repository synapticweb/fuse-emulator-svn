/* if1_fdc.h: IF1 uPD765 floppy disk controller
   Copyright (c) 1999-2011 Philip Kendall, Darren Salt
   Copyright (c) 2012 Alex Badea

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

   Alex: vamposdecampos@gmail.com

*/

#ifndef FUSE_IF1_FDC_H
#define FUSE_IF1_FDC_H

#include "peripherals/disk/fdd.h"

typedef enum if1_drive_number {
	IF1_DRIVE_1 = 0,
	IF1_DRIVE_2 = 1,
} if1_drive_number;

extern int if1_fdc_available;

void if1_fdc_init(void);

int if1_fdc_insert(if1_drive_number which, const char *filename, int autoload);
int if1_fdc_eject(if1_drive_number which);
int if1_fdc_save(if1_drive_number which, int saveas);
int if1_fdc_disk_write(if1_drive_number which, const char *filename);
int if1_fdc_flip(if1_drive_number which, int flip);
int if1_fdc_writeprotect(if1_drive_number which, int wp);
fdd_t *if1_fdc_get_fdd(if1_drive_number which);

#endif				/* #ifndef FUSE_IF1_FDC_H */

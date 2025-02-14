/* kempmouse.h: Kempston mouse emulation
   Copyright (c) 2004-2008 Darren Salt, Fredrick Meunier

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

   E-mail: linux@youmustbejoking.demon.co.uk

*/

#ifndef FUSE_KEMPMOUSE_H
#define FUSE_KEMPMOUSE_H

#include <libspectrum.h>
#include "periph.h"

void kempmouse_init( void );
void kempmouse_update( int dx, int dy, int button, int down );

#endif

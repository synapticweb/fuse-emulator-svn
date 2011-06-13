/* spectranet.h: Spectranet emulation
   Copyright (c) 2011 Philip Kendall

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

   E-mail: philip-fuse@shadowmagic.org.uk

*/

#ifndef FUSE_SPECTRANET_H
#define FUSE_SPECTRANET_H

int spectranet_init( void );
void spectranet_page( void );
void spectranet_unpage( void );

libspectrum_byte spectranet_w5100_read( memory_page *page, libspectrum_word address );
void spectranet_w5100_write( memory_page *page, libspectrum_word address, libspectrum_byte b );
void spectranet_flash_rom_write( libspectrum_word address, libspectrum_byte b );

extern int spectranet_available;
extern int spectranet_paged;
extern int spectranet_w5100_paged_a, spectranet_w5100_paged_b;
extern int spectranet_programmable_trap_active;
extern int spectranet_programmable_trap;

#endif /* #ifndef FUSE_SPECTRANET_H */

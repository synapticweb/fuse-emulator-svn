/* tape.c: Routines for handling .tap files
   Copyright (c) 2001 Philip Kendall

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#include <config.h>

#include <stddef.h>
#include <string.h>

#include "internals.h"

#define DESCRIPTION_LENGTH 256

static libspectrum_error
write_rom( libspectrum_tape_rom_block *block, libspectrum_byte **buffer,
	   libspectrum_byte **ptr, size_t *length );

libspectrum_error
libspectrum_tap_create( libspectrum_tape *tape, const libspectrum_byte *buffer,
			const size_t length )
{
  libspectrum_tape_block *block;
  libspectrum_tape_rom_block *rom_block;
  libspectrum_error error;

  const libspectrum_byte *ptr, *end;

  ptr = buffer; end = buffer + length;

  while( ptr < end ) {
    
    /* If we've got less than two bytes for the length, something's
       gone wrong, so gone home */
    if( ( end - ptr ) < 2 ) {
      libspectrum_tape_free( tape );
      libspectrum_print_error(
        "libspectrum_tap_create: not enough data in buffer\n"
      );
      return LIBSPECTRUM_ERROR_CORRUPT;
    }

    /* Get memory for a new block */
    block = (libspectrum_tape_block*)malloc( sizeof( libspectrum_tape_block ));
    if( block == NULL ) {
      libspectrum_print_error( "libspectrum_tap_create: out of memory\n" );
      return LIBSPECTRUM_ERROR_MEMORY;
    }

    /* This is a standard ROM loader */
    block->type = LIBSPECTRUM_TAPE_BLOCK_ROM;
    rom_block = &(block->types.rom);

    /* Get the length, and move along the buffer */
    rom_block->length = ptr[0] + ptr[1] * 0x100; ptr += 2;

    /* Have we got enough bytes left in buffer? */
    if( end - ptr < (ptrdiff_t)rom_block->length ) {
      libspectrum_tape_free( tape );
      free( block );
      libspectrum_print_error(
        "libspectrum_tap_create: not enough data in buffer\n"
      );
      return LIBSPECTRUM_ERROR_CORRUPT;
    }

    /* Allocate memory for the data */
    rom_block->data = (libspectrum_byte*)malloc( rom_block->length *
						 sizeof( libspectrum_byte ) );
    if( rom_block->data == NULL ) {
      libspectrum_tape_free( tape );
      free( block );
      libspectrum_print_error( "libspectrum_tap_create: out of memory\n" );
      return LIBSPECTRUM_ERROR_MEMORY;
    }

    /* Copy the block data across, and move along */
    memcpy( rom_block->data, ptr, rom_block->length );
    ptr += rom_block->length;

    /* Give a 1s pause after each block */
    rom_block->pause = 1000;

    /* Finally, put the block into the block list */
    tape->blocks = g_slist_append( tape->blocks, (gpointer)block );

  }

  /* And we're pointing to the first block */
  tape->current_block = tape->blocks;

  /* Which we should then initialise */
  error = libspectrum_tape_init_block(
            (libspectrum_tape_block*)tape->current_block->data
          );
  if( error != LIBSPECTRUM_ERROR_NONE ) return error;

  return LIBSPECTRUM_ERROR_NONE;
}

libspectrum_error
libspectrum_tap_write( libspectrum_tape *tape,
		       libspectrum_byte **buffer, size_t *length )
{
  GSList *block_ptr;
  libspectrum_error error;
  char description[ DESCRIPTION_LENGTH ];

  libspectrum_byte *ptr = *buffer;

  for( block_ptr = tape->blocks; block_ptr; block_ptr = block_ptr->next ) {
    libspectrum_tape_block *block = (libspectrum_tape_block*)block_ptr->data;

    switch( block->type ) {

    case LIBSPECTRUM_TAPE_BLOCK_ROM:
      error = write_rom( &(block->types.rom), buffer, &ptr, length );
      if( error != LIBSPECTRUM_ERROR_NONE ) { free( *buffer ); return error; }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_COMMENT:
    error = libspectrum_tape_block_description( block, description,
						DESCRIPTION_LENGTH );
    if( error ) { free( *buffer ); return 1; }

    libspectrum_print_error(
      "libspectrum_tap_write: skipping %s (ID 0x%02x)\n", description,
      block->type
    );
    break;

    default:
      if( *length ) free( *buffer );
      libspectrum_print_error(
        "libspectrum_tap_write: unknown block type 0x%02x\n", block->type
      );
      return LIBSPECTRUM_ERROR_LOGIC;

    }
  }

  (*length) = ptr - *buffer;

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
write_rom( libspectrum_tape_rom_block *block, libspectrum_byte **buffer,
	   libspectrum_byte **ptr, size_t *length )
{
  libspectrum_error error;

  /* Make room the length and the actual data */
  error = libspectrum_make_room( buffer, 2 + block->length, ptr, length );
  if( error != LIBSPECTRUM_ERROR_NONE ) return error;

  /* Write out the length and the data */
  *(*ptr)++ = block->length & 0xff;
  *(*ptr)++ = ( block->length & 0xff00 ) >> 8;
  memcpy( *ptr, block->data, block->length ); (*ptr) += block->length;

  return LIBSPECTRUM_ERROR_NONE;
}

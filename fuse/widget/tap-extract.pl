#!/usr/bin/perl -w

# tap-extract.pl: generate the keyboard picture given a .tap SCREEN$ of it
# Copyright (c) 2002 Philip Kendall

# $Id$

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA

# Author contact information:

# E-mail: pak21-fuse@srcf.ucam.org
# Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

use strict;

use IO::File;

die "usage: $0 <tapefile>" unless @ARGV == 1;

sub read_block ($) {

    my $fh = shift;

    my $buffer;
    read( $fh, $buffer, 2 ) or return undef;

    my $length = ord( substr( $buffer, 0, 1 ) ) +
	256 * ord( substr( $buffer, 1, 1 ) );

    read( $fh, $buffer, $length ) or return undef;

    return { length => $length, data => $buffer };

}

my $tapefile = shift;

my $fh = new IO::File "< $tapefile" or die "couldn't open '$tapefile': $!";

my $header = read_block( $fh ) or die "read_block error: $!";
die "Not a header" unless $header->{length} == 19;

$header= read_block( $fh ) or die "read_block error: $!";
die "Not a screen" unless $header->{length} == 6914;

$fh->close or die "couldn't close '$tapefile': $!";

# Remove flag and parity bytes
substr( $header->{data}, 0, 1 ) = '';
substr( $header->{data}, -1, 1 ) = '';

# Extract the data into a sensible form
my $data = [];
foreach( my $third = 0; $third < 3; $third++ ) {
    foreach( my $pixel_line = 0; $pixel_line < 8; $pixel_line++ ) {
	foreach( my $row = 0; $row < 8; $row++ ) {

	    my $y = $third * 64 + $row * 8 + $pixel_line;
	    
	    foreach( my $x=0; $x < 32; $x++ ) {
		$data->[$y][$x] = ord( substr( $header->{data}, 0, 1, '' ) );
	    }
	}
    }
}

# And then the attributes
my( $ink, $paper )  = ( [], [] );
foreach( my $y=0; $y<24; $y++ ) {
    foreach( my $x=0; $x<32; $x++ ) {
	my $attribute = ord( substr( $header->{data}, 0, 1, '' ) );

	$ink->[$y][$x] = ( $attribute&0x07 ) + ( ( $attribute&0x40 ) >> 3 );
	$paper->[$y][$x] = ( $attribute & ( 0x0f << 3 ) ) >> 3;

    }
}

print << "CODE";
/* picture_data.c: Keyboard picture data
   Copyright (c) 2001,2002 Russell Marks, Philip Kendall

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

   E-mail: pak21-fuse\@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

/* This file is autogenerated from picture-data.tap by tap-extract.pl
   Do not edit unless you know what you\'re doing! */

#include <config.h>

#include "display.h"

int widget_picture_data[DISPLAY_HEIGHT][DISPLAY_WIDTH] = {
CODE

foreach( my $y=0; $y<192; $y++ ) {

    print "  { ";

    foreach( my $col=0; $col<32; $col++ ) {
	foreach( my $pixel=0; $pixel<8; $pixel++ ) {

	    if( $data->[$y][$col] & 0x80 ) {
		print $ink->[$y/8][$col];
	    } else {
		print $paper->[$y/8][$col];
	    }
	    print ", ";

	    $data->[$y][$col] <<= 1;
	}
    }

    print "},\n";
}

print "};\n";

/*
 Copyright (C) 1996-1997  Id Software, Inc.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to:

  Free Software Foundation, Inc.
  59 Temple Place - Suite 330
  Boston, MA  02111-1307, USA
*/
#include "g_local.h"
gedict_t       *lastspawn;
void SP_worldspawn()
{
	char           *s;

	G_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) )
	{
		G_Error( "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}
	world->s.v.classname = "worldspawn";
	lastspawn = world;
	trap_precache_model( "progs/player.mdl" );
//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//

	// 0 normal
	trap_lightstyle( 0, "m" );

	// 1 FLICKER (first variety)
	trap_lightstyle( 1, "mmnmmommommnonmmonqnmmo" );

	// 2 SLOW STRONG PULSE
	trap_lightstyle( 2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba" );

	// 3 CANDLE (first variety)
	trap_lightstyle( 3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg" );

	// 4 FAST STROBE
	trap_lightstyle( 4, "mamamamamama" );

	// 5 GENTLE PULSE 1
	trap_lightstyle( 5, "jklmnopqrstuvwxyzyxwvutsrqponmlkj" );

	// 6 FLICKER (second variety)
	trap_lightstyle( 6, "nmonqnmomnmomomno" );

	// 7 CANDLE (second variety)
	trap_lightstyle( 7, "mmmaaaabcdefgmmmmaaaammmaamm" );

	// 8 CANDLE (third variety)
	trap_lightstyle( 8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa" );

	// 9 SLOW STROBE (fourth variety)
	trap_lightstyle( 9, "aaaaaaaazzzzzzzz" );

	// 10 FLUORESCENT FLICKER
	trap_lightstyle( 10, "mmamammmmammamamaaamammma" );

	// 11 SLOW PULSE NOT FADE TO BLACK
	trap_lightstyle( 11, "abcdefghijklmnopqrrqponmlkjihgfedcba" );

	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	trap_lightstyle( 63, "a" );

}

int framecount;
void StartFrame( int time )
{
	framecount = framecount + 1;
}

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
#define START_OFF 1                 // Light on/off spawnflag
void Light_setup();                  // Definition from Lights.qc

void SP_light()                       // Basic Light
{
	Light_setup();                          // Setup Light
}; 

void light_fluoro()                // Light with hum ambient
{
	Light_setup();                          // Setup Light
};

void light_fluorospark()         // Light with buzz ambient
{
	Light_setup();                          // Setup Light
};                      
void Light_setup()   // Set light on or off, as per spawnflags
{
	if (self->style < 32) {return;} // Dont switch other styles.

	if ( ( int ) self->s.v.spawnflags & START_OFF )
		trap_lightstyle(self->style, "a");    // If light starts off, set it off.
	else
		trap_lightstyle(self->style, "m");    // If light starts ON, turn in ON. Simple :)
};

void LightStyles_setup()
{
	// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.

	trap_lightstyle(0,"m");                                   // Style 0: Normal
	trap_lightstyle(1,"mmnmmommommnonmmonqnmmo");             // Style 1: Flicker
													 // Style 2: Slow Strong
													 //          Pulse
	trap_lightstyle(2,"abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");
	trap_lightstyle(3,"mmmmmaaaaammmmmaaaaaabcdefgabcdefg");  // Style 3: Candle
	trap_lightstyle(4,"mamamamamama");                        // Style 4: Fast Strobe
	trap_lightstyle(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");    // Style 5: Gentle Pulse
	trap_lightstyle(6,"nmonqnmomnmomomno");                   // Style 6: Flicker 2
	trap_lightstyle(7,"mmmaaaabcdefgmmmmaaaammmaamm");        // Style 7: Candle 2
													 // Style 8: Candle 3
	trap_lightstyle(8,"mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa"); 
	trap_lightstyle(9,"aaaaaaaazzzzzzzz");                    // Style 9: Slow Strobe
	trap_lightstyle(10,"mmamammmmammamamaaamammma");          // Style 10: Fluro
													 // Style 11: Slow Pulse
													 //           (no black)
	trap_lightstyle(11,"abcdefghijklmnopqrrqponmlkjihgfedcba"); 
};
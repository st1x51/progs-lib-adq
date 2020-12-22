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
vec3_t          VEC_ORIGIN = { 0, 0, 0 };
vec3_t          VEC_HULL_MIN = { -16, -16, -24 };
vec3_t          VEC_HULL_MAX = { 16, 16, 32 };

vec3_t          VEC_HULL2_MIN = { -32, -32, -24 };
vec3_t          VEC_HULL2_MAX = { 32, 32, 64 };
//for hl maps
vec3_t          VEC_HULLHL_MIN = { -16, -16, -36 };
vec3_t          VEC_HULLHL_MAX = { 16, 16, 36 };
vec3_t			VEC_DEFAULT_VIEW_SIZE = {0, 0, 28};
int             modelindex_player;

/*
=============================================================================

    LEVEL CHANGING / INTERMISSION

=============================================================================
*/
void SetChangeParms (void)
{

}
void SetNewParms( void )
{
	g_globalvars.parm1 = 0;
	g_globalvars.parm2 = 100;
	g_globalvars.parm3 = 0;
	g_globalvars.parm4 = 0;
	g_globalvars.parm5 = 0;
	g_globalvars.parm6 = 0;
	g_globalvars.parm7 = 0;
	g_globalvars.parm8 = 1;
	g_globalvars.parm9 = 0;
}

/*
=============================================================================

    PLAYER GAME EDGE FUNCTIONS

=============================================================================
*/

// called by ClientKill and DeadThink
void respawn()
{
	// set default spawn parms
	SetNewParms();
	// respawn              
	PutClientInServer();
}


/*
============
ClientKill

Player entered the suicide command
============
*/
void ClientKill()
{
	G_bprint("%s suicides\n", self->s.v.netname );
	respawn();
}


////////////////
// GlobalParams:
// time
// self
// params
///////////////
void ClientConnect()
{
	G_bprint("%s entered the game\n", self->s.v.netname );
}

////////////////
// GlobalParams:
// time
// self
// called after ClientConnect
///////////////
void PutClientInServer()
{

	gedict_t       *spot;
	vec3_t          v;
	int             items;
	char            s[20];

	self->s.v.classname = "player";
	self->s.v.health = 100;
	self->s.v.groundentity = 1;
	self->s.v.takedamage = DAMAGE_AIM;
	self->s.v.solid = SOLID_SLIDEBOX;
	self->s.v.movetype = MOVETYPE_WALK;
	self->s.v.flags = FL_CLIENT;
	self->s.v.deadflag = DEAD_NO;
	spot = find( world, FOFS( s.v.classname ), "info_player_start" );
	VectorCopy( spot->s.v.origin, self->s.v.origin );
	self->s.v.origin[2] += 1;
	VectorCopy( spot->s.v.angles, self->s.v.angles );
	self->s.v.fixangle = true;
	self->s.v.hull = 2;
	setmodel( self, "progs/player.mdl" );
	modelindex_player = self->s.v.modelindex;

	setsize( self, PASSVEC3( VEC_HULLHL_MIN ), PASSVEC3( VEC_HULLHL_MAX ) );
	SetVector( self->s.v.view_ofs, 0, 0, 28 );
	SetVector( self->s.v.velocity, 0, 0, 0 );
}

void PlayerJump()
{
	if ( ( ( int ) ( self->s.v.flags ) ) & FL_WATERJUMP )
		return;

	if ( self->s.v.waterlevel >= 2 )
	{
		if(self->s.v.watertype == CONTENT_WATER)
			self->s.v.velocity[2] = 100;
		else if (self->s.v.watertype == CONTENT_SLIME)
			self->s.v.velocity[2] = 80;
		else
			self->s.v.velocity[2] = 50;
		return;
	}

	if ( !( ( ( int ) ( self->s.v.flags ) ) & FL_ONGROUND ) )
		return;

	if ( !( ( ( int ) ( self->s.v.flags ) ) & FL_JUMPRELEASED ) )
		return;		// don't pogo stick

	self->s.v.flags -= ( ( ( int ) ( self->s.v.flags ) ) & FL_JUMPRELEASED );
	self->s.v.button2 = 0;
	self->s.v.velocity[2] += 270;
}


/*
===========
WaterMove

============
*/

void WaterMove()
{
//dprint (ftos(self->s.v.waterlevel));
	if ( self->s.v.movetype == MOVETYPE_NOCLIP )
		return;

	if ( self->s.v.health < 0 )
		return;

	if ( self->s.v.waterlevel != 3 )
	{
		if ( self->air_finished < g_globalvars.time )
			sound( self, CHAN_VOICE, "player/gasp2.wav", 1, ATTN_NORM );
		else if ( self->air_finished < g_globalvars.time + 9 )
			sound( self, CHAN_VOICE, "player/gasp1.wav", 1, ATTN_NORM );

		self->air_finished = g_globalvars.time + 12;
		self->dmg = 2;

	} else if ( self->air_finished < g_globalvars.time )
	{			// drown!
		if ( self->pain_finished < g_globalvars.time )
		{
			self->dmg = self->dmg + 2;
			if ( self->dmg > 15 )
				self->dmg = 10;

			T_Damage( self, world, world, self->dmg );
			self->pain_finished = g_globalvars.time + 1;
		}
	}

	if ( !self->s.v.waterlevel )
	{
		if ( ( ( int ) ( self->s.v.flags ) ) & FL_INWATER )
		{
			// play leave water sound
			sound( self, CHAN_BODY, "misc/outwater.wav", 1, ATTN_NORM );
			self->s.v.flags -= FL_INWATER;
		}
		return;
	}

	if ( self->s.v.watertype == CONTENT_LAVA )
	{			// do damage
		if ( self->dmgtime < g_globalvars.time )
		{
			if ( self->radsuit_finished > g_globalvars.time )
				self->dmgtime = g_globalvars.time + 1;
			else
				self->dmgtime = g_globalvars.time + 0.2;

			T_Damage( self, world, world, 10 * self->s.v.waterlevel );
		}

	} else if ( self->s.v.watertype == CONTENT_SLIME )
	{			// do damage
		if ( self->dmgtime < g_globalvars.time
		     && self->radsuit_finished < g_globalvars.time )
		{
			self->dmgtime = g_globalvars.time + 1;
			T_Damage( self, world, world, 4 * self->s.v.waterlevel );
		}
	}

	if ( !( ( ( int ) ( self->s.v.flags ) ) & FL_INWATER ) )
	{

// player enter water sound

		if ( self->s.v.watertype == CONTENT_LAVA )
			sound( self, CHAN_BODY, "player/inlava.wav", 1, ATTN_NORM );
		if ( self->s.v.watertype == CONTENT_WATER )
			sound( self, CHAN_BODY, "player/inh2o.wav", 1, ATTN_NORM );
		if ( self->s.v.watertype == CONTENT_SLIME )
			sound( self, CHAN_BODY, "player/slimbrn2.wav", 1,
				    ATTN_NORM );

		self->s.v.flags += FL_INWATER;
		self->dmgtime = 0;
	}
}

////////////////
// GlobalParams:
// self
///////////////
void ClientDisconnect()
{
	G_bprint("%s left the game with %.0f frags\n", self->s.v.netname,
		  self->s.v.frags );

	sound( self, CHAN_BODY, "player/tornoff2.wav", 1, ATTN_NONE );
}

////////////////
// GlobalParams:
// time
// frametime
// self
///////////////
/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void PlayerPreThink()
{
	WaterMove();
}

////////////////
// GlobalParams:
// time
// self
///////////////
void PlayerPostThink()
{
	if ( self->s.v.button2 )
	{
		PlayerJump();
	} 
	else
		self->s.v.flags = ( ( int ) ( self->s.v.flags ) ) | FL_JUMPRELEASED;
}


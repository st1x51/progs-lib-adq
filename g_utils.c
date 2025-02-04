#include "g_local.h"


int NUM_FOR_EDICT( gedict_t * e )
{
	int     b;

	b = ( byte * ) e - ( byte * ) g_edicts;
	b = b / sizeof( gedict_t );

	if ( b < 0 || b >= MAX_EDICTS )
		G_Error( "NUM_FOR_EDICT: bad pointer" );
	return b;
}

float bound( float a, float b, float c )
{
	return ( a >= c ? a : b < a ? a : b > c ? c : b);
}

float g_random(  )
{
	return ( rand(  ) & 0x7fff ) / ( ( float ) 0x7fff );
}

float crandom(  )
{
	return 2 * ( g_random(  ) - 0.5 );
}

int i_rnd( int from, int to )
{
	float r;

	if ( from >= to )
		return from;

	r = (int)(from + (1.0 + to - from) * g_random());

	return bound(from, r, to);
}

gedict_t *spawn(  )
{
	gedict_t *t = &g_edicts[trap_spawn(  )];

	if ( !t || t == world )
		DebugTrap( "spawn return world\n" );
	return t;
}

void ent_remove( gedict_t * t )
{
	if ( !t || t == world )
		DebugTrap( "BUG BUG BUG remove world\n" );
	trap_remove( NUM_FOR_EDICT( t ) );
}



gedict_t *nextent( gedict_t * ent )
{
	int     entn;

	if ( !ent )
		G_Error( "find: NULL start\n" );
	entn = trap_nextent( NUM_FOR_EDICT( ent ) );
	if ( entn )
		return &g_edicts[entn];
	else
		return NULL;
}

gedict_t *find( gedict_t * start, int fieldoff, char *str )
{
    return (gedict_t*) trap_find( start, fieldoff, str );
	/*gedict_t *e;
	char   *s;

	if ( !start )
		G_Error( "find: NULL start\n" );
	for ( e = nextent( start ); e; e = nextent( e ) )
	{
		s = *( char ** ) ( ( byte * ) e + fieldoff );
		if ( s && !strcmp( s, str ) )
			return e;
	}
	return NULL;*/
}


void normalize( vec3_t value, vec3_t newvalue )
{
	float   new;


	new = value[0] * value[0] + value[1] * value[1] + value[2] * value[2];
	new = sqrt( new );

	if ( new == 0 )
		value[0] = value[1] = value[2] = 0;
	else
	{
		new = 1 / new;
		newvalue[0] = value[0] * new;
		newvalue[1] = value[1] * new;
		newvalue[2] = value[2] * new;
	}

}
void aim( vec3_t ret )
{
	VectorCopy( g_globalvars.v_forward, ret );
}

char    null_str[] = "";
int streq( char *s1, char *s2 )
{
	if ( !s1 )
		s1 = null_str;
	if ( !s2 )
		s2 = null_str;
	return ( !strcmp( s1, s2 ) );
}
int strneq( char *s1, char *s2 )
{
	if ( !s1 )
		s1 = null_str;
	if ( !s2 )
		s2 = null_str;

	return ( strcmp( s1, s2 ) );
}

/*
=================
vlen

scalar vlen(vector)
=================
*/
float vlen( vec3_t value1 )
{
	float   new;


	new = value1[0] * value1[0] + value1[1] * value1[1] + value1[2] * value1[2];
	new = sqrt( new );

	return new;
}

float vectoyaw( vec3_t value1 )
{
	float   yaw;


	if ( value1[1] == 0 && value1[0] == 0 )
		yaw = 0;
	else
	{
		yaw = ( int ) ( atan2( value1[1], value1[0] ) * 180 / M_PI );
		if ( yaw < 0 )
			yaw += 360;
	}

	return yaw;
}


void vectoangles( vec3_t value1, vec3_t ret )
{
	float   forward;
	float   yaw, pitch;


	if ( value1[1] == 0 && value1[0] == 0 )
	{
		yaw = 0;
		if ( value1[2] > 0 )
			pitch = 90;
		else
			pitch = 270;
	} else
	{
		yaw = ( int ) ( atan2( value1[1], value1[0] ) * 180 / M_PI );
		if ( yaw < 0 )
			yaw += 360;

		forward = sqrt( value1[0] * value1[0] + value1[1] * value1[1] );
		pitch = ( int ) ( atan2( value1[2], forward ) * 180 / M_PI );
		if ( pitch < 0 )
			pitch += 360;
	}


	ret[0] = pitch;
	ret[1] = yaw;
	ret[2] = 0;
}


/*
=================
PF_findradius

Returns a chain of entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
gedict_t *findradius( gedict_t * start, vec3_t org, float rad )
{
	gedict_t *ent;
	vec3_t  eorg;
	int     j;

	for ( ent = nextent( start ); ent; ent = nextent( ent ) )
	{
		if ( ent->s.v.solid == SOLID_NOT )
			continue;
		for ( j = 0; j < 3; j++ )
			eorg[j] =
			    org[j] - ( ent->s.v.origin[j] +
				       ( ent->s.v.mins[j] + ent->s.v.maxs[j] ) * 0.5 );
		if ( VectorLength( eorg ) > rad )
			continue;
		return ent;

	}
	return NULL;

}


/*
==============
changeyaw

This was a major timewaster in progs, so it was converted to C
==============
*/
void changeyaw( gedict_t * ent )
{
	float   ideal, current, move, speed;

	current = anglemod( ent->s.v.angles[1] );
	ideal = ent->s.v.ideal_yaw;
	speed = ent->s.v.yaw_speed;

	if ( current == ideal )
		return;
	move = ideal - current;
	if ( ideal > current )
	{
		if ( move >= 180 )
			move = move - 360;
	} else
	{
		if ( move <= -180 )
			move = move + 360;
	}
	if ( move > 0 )
	{
		if ( move > speed )
			move = speed;
	} else
	{
		if ( move < -speed )
			move = -speed;
	}

	ent->s.v.angles[1] = anglemod( current + move );
}

/*
==============
PF_makevectors

Writes new values for v_forward, v_up, and v_right based on angles
makevectors(vector)
==============
*/

void makevectors( vec3_t vector )
{
	AngleVectors( vector, g_globalvars.v_forward, g_globalvars.v_right,
		      g_globalvars.v_up );
}


/*
==============
print functions
==============
*/
void G_sprint( gedict_t * ed, const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_SPrint( NUM_FOR_EDICT( ed ), text );
}

void G_bprint(const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_BPrint(text );
}

void G_centerprint( gedict_t * ed, const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_CenterPrint( NUM_FOR_EDICT( ed ), text );
}

void G_dprint( const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_DPrintf( text );
}

void localcmd( const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_localcmd( text );
}

void stuffcmd( gedict_t * ed, const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_stuffcmd( NUM_FOR_EDICT( ed ), text );
}


void setorigin( gedict_t * ed, float origin_x, float origin_y, float origin_z )
{
	trap_setorigin( NUM_FOR_EDICT( ed ), origin_x, origin_y, origin_z );
}

void setsize( gedict_t * ed, float min_x, float min_y, float min_z, float max_x,
	      float max_y, float max_z )
{
	trap_setsize( NUM_FOR_EDICT( ed ), min_x, min_y, min_z, max_x, max_y, max_z );
}

void setmodel( gedict_t * ed, char *model )
{
	trap_setmodel( NUM_FOR_EDICT( ed ), model );
}

void sound( gedict_t * ed, int channel, char *samp, int vol, float att )
{
	trap_sound( NUM_FOR_EDICT( ed ), channel, samp, vol, att );
}

gedict_t *checkclient(  )
{
	return &g_edicts[trap_checkclient(  )];
}
void traceline( float v1_x, float v1_y, float v1_z, float v2_x, float v2_y,
		float v2_z, int nomonst, gedict_t * ed )
{
	trap_traceline( v1_x, v1_y, v1_z, v2_x, v2_y, v2_z, nomonst,
			NUM_FOR_EDICT( ed ) );
}


int droptofloor( gedict_t * ed )
{
	return trap_droptofloor( NUM_FOR_EDICT( ed ) );
}

int walkmove( gedict_t * ed, float yaw, float dist )
{
	return trap_walkmove( NUM_FOR_EDICT( ed ), yaw, dist );
}

int checkbottom( gedict_t * ed )
{
	return trap_checkbottom( NUM_FOR_EDICT( ed ) );
}

void makestatic( gedict_t * ed )
{
	trap_makestatic( NUM_FOR_EDICT( ed ) );
}

void setspawnparam( gedict_t * ed )
{
	trap_setspawnparam( NUM_FOR_EDICT( ed ) );
}

void logfrag( gedict_t * killer, gedict_t * killee )
{
	trap_logfrag( NUM_FOR_EDICT( killer ), NUM_FOR_EDICT( killee ) );
}

void infokey( gedict_t * ed, char *key, char *valbuff, int sizebuff )
{
	trap_infokey( NUM_FOR_EDICT( ed ), key, valbuff, sizebuff );
}

void WriteEntity( int to, gedict_t * ed )
{
	trap_WriteEntity( to, NUM_FOR_EDICT( ed ) );
}

void disableupdates( gedict_t * ed, float time )
{
	trap_disableupdates( NUM_FOR_EDICT( ed ), time );
}

#include "g_local.h"
gedict_t        g_edicts[MAX_EDICTS];
gedict_t       *world = g_edicts;
gedict_t       *self, *other;

//short shortvar=0xfedc;
globalvars_t    g_globalvars;
field_t         expfields[] = {
	{"maxspeed", FOFS( maxspeed ), F_FLOAT}	,
	{"gravity", FOFS( gravity ), F_FLOAT},
	{NULL}
};
static char     mapname[64];
static char     worldmodel[64] = "worldmodel";
static char     netnames[MAX_CLIENTS][32];

gameData_t      gamedata =
    { ( edict_t * ) g_edicts, sizeof( gedict_t ), &g_globalvars, expfields , GAME_API_VERSION};
float           starttime;
void            G_InitGame( int levelTime, int randomSeed );
void            StartFrame( int time );
qboolean        ClientCommand();
qboolean        ClientUserInfoChanged();
void            G_EdictTouch();
void            G_EdictThink();
void            G_EdictBlocked();
void            ClearGlobals();

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5,
	    int arg6, int arg7, int arg8, int arg9, int arg10, int arg11 )
{
	int api_ver;
	ClearGlobals();
	switch ( command )
	{
	case GAME_INIT:
        api_ver = trap_GetApiVersion();
			G_Printf("%d \n",sizeof(entvars_t));
                G_dprint( "Init Game %d %d %d %d %d\n", api_ver, GAME_API_VERSION, sizeof( edict_t), sizeof( entvars_t), sizeof( edict_t)- sizeof( entvars_t) );
		if (  api_ver < GAME_API_VERSION )
			return 0;

		G_InitGame( arg0, arg1 );
		return ( int ) ( &gamedata );

	case GAME_LOADENTS:
		G_SpawnEntitiesFromString();
		return 1;

	case GAME_START_FRAME:
		StartFrame( arg0 );
		return 1;

	case GAME_CLIENT_CONNECT:
		self = PROG_TO_EDICT( g_globalvars.self );
			ClientConnect();
		return 1;

	case GAME_PUT_CLIENT_IN_SERVER:
		self = PROG_TO_EDICT( g_globalvars.self );
			PutClientInServer();
		return 1;

	case GAME_CLIENT_DISCONNECT:
		self = PROG_TO_EDICT( g_globalvars.self );
			ClientDisconnect();
		return 1;

	case GAME_SETNEWPARMS:
		SetNewParms();
		return 1;

	case GAME_CLIENT_PRETHINK:
		self = PROG_TO_EDICT( g_globalvars.self );
			PlayerPreThink();
		return 1;

	case GAME_CLIENT_POSTTHINK:
		self = PROG_TO_EDICT( g_globalvars.self );
			PlayerPostThink();
		return 1;

	case GAME_EDICT_TOUCH:
		G_EdictTouch();
		return 1;

	case GAME_EDICT_THINK:
		G_EdictThink();
		return 1;

	case GAME_EDICT_BLOCKED:
		G_EdictBlocked();
		return 1;

	case GAME_SETCHANGEPARMS:
		self = PROG_TO_EDICT( g_globalvars.self );
		SetChangeParms();
		return 1;
//TODO:Check this code,idk if this works
	case GAME_CLIENT_COMMAND:
		return ClientCommand();

	case GAME_SHUTDOWN:
		return 0;
//TODO:Check this code,idk if this works
	case GAME_CLIENT_USERINFO_CHANGED:
		ClientUserInfoChanged( arg0 );
		return 0;
	}
	return 0;
}

//===================================================================
void G_Printf( const char *fmt, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_DPrintf( text );
}

void G_Error( const char *fmt, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_Error( text );
}

void Com_Error( int level, const char *error, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start( argptr, error );
	vsprintf( text, error, argptr );
	va_end( argptr );

	G_Error( "%s", text );
}

void Com_Printf( const char *msg, ... )
{
	va_list         argptr;
	char            text[1024];

	va_start( argptr, msg );
	vsprintf( text, msg, argptr );
	va_end( argptr );

	G_Printf( "%s", text );
}

//===================================================================
void G_InitGame( int levelTime, int randomSeed )
{
	int             i;

	srand( randomSeed );
	framecount = 0;
	starttime = levelTime * 0.001;
	G_InitMemory();
	memset( g_edicts, 0, sizeof( gedict_t ) * MAX_EDICTS );


	world->s.v.model = worldmodel;
	g_globalvars.mapname = mapname;
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		g_edicts[i + 1].s.v.netname = netnames[i];
	}
}




//===========================================================================
// Physics
// 
//===========================================================================
////////////////
// GlobalParams:
// time
// self
// other
///////////////
void G_EdictTouch()
{
	self = PROG_TO_EDICT( g_globalvars.self );
	other = PROG_TO_EDICT( g_globalvars.other );
	if ( self->s.v.touch )
	{
/*#ifdef DEBUG
	        if(self->s.v.classname && other->s.v.classname)
	        	if(!strcmp(self->s.v.classname,"player")||!strcmp(other->s.v.classname,"player"))
	         G_dprint( "touch %s <-> %s\n", self->s.v.classname,other->s.v.classname);
#endif*/
		( ( void ( * )() ) ( self->s.v.touch ) ) ();
	} else
	{
		G_Printf( "Null touch func" );
	}
}

////////////////
// GlobalParams:
// time
// self
// other=world
///////////////
void G_EdictThink()
{
	self = PROG_TO_EDICT( g_globalvars.self );
	other = PROG_TO_EDICT( g_globalvars.other );
	if ( self->s.v.think )
	{
		( ( void ( * )() ) ( self->s.v.think ) ) ();
	} else
	{
		G_Printf( "Null think func" );
	}

}

////////////////
// GlobalParams:
// time
// self=pusher
// other=check
// if the pusher has a "blocked" function, call it
// otherwise, just stay in place until the obstacle is gone
///////////////
void G_EdictBlocked()
{
	self = PROG_TO_EDICT( g_globalvars.self );
	other = PROG_TO_EDICT( g_globalvars.other );

	if ( self->s.v.blocked )
	{
		( ( void ( * )() ) ( self->s.v.blocked ) ) ();
	} else
	{
		//G_Printf("Null blocked func");
	}

}


void ClearGlobals()
{
	damage_attacker = damage_inflictor = activator = self = other = world;
}

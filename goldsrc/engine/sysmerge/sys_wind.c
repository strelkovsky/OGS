/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
 *	Copyright (C) 2018 BlackPhrase
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file
/// @brief null system driver to aid porting efforts

#include "quakedef.h"
#include "winquake.h"
#include "errno.h"
#include <sys\types.h>
#include <sys\timeb.h>
//#include <winsock.h>
//#include <conio.h>

//cvar_t	sys_nostdout = {"sys_nostdout","0"};

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES 10
FILE *sys_handles[MAX_HANDLES];

int findhandle()
{
	int i;

	for(i = 1; i < MAX_HANDLES; i++)
		if(!sys_handles[i])
			return i;
	Sys_Error("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength(FILE *f)
{
	int pos;
	int end;

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead(char *path, int *hndl)
{
	FILE *f;
	int i;

	i = findhandle();

	f = fopen(path, "rb");
	if(!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;

	return filelength(f);
}

int Sys_FileOpenWrite(char *path)
{
	FILE *f;
	int i;

	i = findhandle();

	f = fopen(path, "wb");
	if(!f)
		Sys_Error("Error opening %s: %s", path, strerror(errno));
	sys_handles[i] = f;

	return i;
}

void Sys_FileClose(int handle)
{
	fclose(sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek(int handle, int position)
{
	fseek(sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead(int handle, void *dest, int count)
{
	return fread(dest, 1, count, sys_handles[handle]);
}

int Sys_FileWrite(int handle, void *data, int count)
{
	return fwrite(data, 1, count, sys_handles[handle]);
}

int Sys_FileTime(char *path)
{
	FILE *f;

	f = fopen(path, "rb");
	if(f)
	{
		fclose(f);
		return 1;
	}

	return -1;
}

void Sys_mkdir(char *path)
{
}

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length)
{
}

void Sys_DebugLog(char *file, char *fmt, ...)
{
}

void Sys_Error(char *error, ...)
{
	va_list argptr;
	char text[1024];

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	//    MessageBox(NULL, text, "Error", 0 /* MB_OK */ );
	printf("ERROR: %s\n", text);

	exit(1);
}

/*
================
Sys_Printf
================
*/
void Sys_Printf(char *fmt, ...)
{
	va_list argptr;

	//if (sys_nostdout.value)
		//return;
	
	va_start(argptr, fmt);
	vprintf(fmt, argptr);
	va_end(argptr);
}

/*
================
Sys_Quit
================
*/
void Sys_Quit()
{
	exit(0);
}

/*
=============
Sys_Init

Engine calls this so the system can register variables before host_hunklevel
is marked
=============
*/
void Sys_Init ()
{
	Cvar_RegisterVariable (&sys_nostdout);
}

double Sys_FloatTime()
{
	double t;
	struct _timeb tstruct;
	static int starttime;

	_ftime(&tstruct);

	if(!starttime)
		starttime = tstruct.time;
	t = (tstruct.time - starttime) + tstruct.millitm * 0.001;

	return t;
}

void Sys_Sleep()
{
}

void Sys_SendKeyEvents()
{
}

void Sys_HighFPPrecision()
{
}

void Sys_LowFPPrecision()
{
}

char *Sys_ConsoleInput()
{
	static char text[256];
	static int len;
	int c;

	// read a line out
	while(_kbhit())
	{
		c = _getch();
		putch(c);
		if(c == '\r')
		{
			text[len] = 0;
			putch('\n');
			len = 0;
			return text;
		}
		if(c == 8)
		{
			//if (len) // TODO
			{
				putch(' ');
				putch(c);
				len--;
				text[len] = 0;
			}
			continue;
		}
		text[len] = c;
		len++;
		text[len] = 0;
		if(len == sizeof(text))
			len = 0;
	}

	return NULL;
}

/*
==================
main

==================
*/
char *newargv[256];

int main(int argc, char **argv)
{
	MSG msg;
	quakeparms_t parms;
	double time, oldtime;
	static char cwd[1024];

	memset(&parms, 0, sizeof(parms));

	parms.memsize = 16384 * 1024;
	parms.membase = malloc(parms.memsize);

	_getcwd(cwd, sizeof(cwd));
	if(cwd[Q_strlen(cwd) - 1] == '\\')
		cwd[Q_strlen(cwd) - 1] = 0;
	parms.basedir = cwd; //"f:/quake";
	                     //	parms.basedir = "f:\\quake";

	COM_InitArgv(argc, argv);

	// dedicated server ONLY!
	if(!COM_CheckParm("-dedicated"))
	{
		memcpy(newargv, argv, argc * 4);
		newargv[argc] = "-dedicated";
		argc++;
		argv = newargv;
		COM_InitArgv(argc, argv);
	}

	parms.argc = argc;
	parms.argv = argv;

	printf("Host_Init\n");
	Host_Init(&parms);

	oldtime = Sys_FloatTime();

	/* main window message loop */
	while(1)
	{
		time = Sys_FloatTime();
		if(time - oldtime < sys_ticrate.value)
		{
			Sleep(1);
			continue;
		}

		Host_Frame(time - oldtime);
		oldtime = time;
	}

	/* return success of application */
	return TRUE;
}

// TODO: qw
/*
int main (int argc, char **argv)
{
	quakeparms_t	parms;
	double			newtime, time, oldtime;
	static	char	cwd[1024];
	struct timeval	timeout;
	fd_set			fdset;
	int				t;

	COM_InitArgv (argc, argv);
	
	parms.argc = com_argc;
	parms.argv = com_argv;

	parms.memsize = 16*1024*1024;

	if ((t = COM_CheckParm ("-heapsize")) != 0 &&
		t + 1 < com_argc)
		parms.memsize = Q_atoi (com_argv[t + 1]) * 1024;

	if ((t = COM_CheckParm ("-mem")) != 0 &&
		t + 1 < com_argc)
		parms.memsize = Q_atoi (com_argv[t + 1]) * 1024 * 1024;

	parms.membase = malloc (parms.memsize);

	if (!parms.membase)
		Sys_Error("Insufficient memory.\n");

	parms.basedir = ".";
	parms.cachedir = NULL;

	SV_Init (&parms);

// run one frame immediately for first heartbeat
	SV_Frame (0.1);		

//
// main loop
//
	oldtime = Sys_DoubleTime () - 0.1;
	while (1)
	{
	// select on the net socket and stdin
	// the only reason we have a timeout at all is so that if the last
	// connected client times out, the message would not otherwise
	// be printed until the next event.
		FD_ZERO(&fdset);
		FD_SET(net_socket, &fdset);
		timeout.tv_sec = 0;
		timeout.tv_usec = 100;
		if (select (net_socket+1, &fdset, NULL, NULL, &timeout) == -1)
			continue;

	// find time passed since last cycle
		newtime = Sys_DoubleTime ();
		time = newtime - oldtime;
		oldtime = newtime;
		
		SV_Frame (time);				
	}	

	return true;
}
*/
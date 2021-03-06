/*
 * PowerControl.cpp - namespace PowerControl
 *
 * Copyright (c) 2017 Tobias Doerffel <tobydox/at/users/dot/sf/dot/net>
 *
 * This file is part of iTALC - http://italc.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <italcconfig.h>

#ifdef ITALC_BUILD_WIN32

#define UNICODE
#include <winsock2.h>
#include <windows.h>

#define SHUTDOWN_FLAGS (EWX_FORCE | EWX_FORCEIFHUNG)
#define SHUTDOWN_REASON SHTDN_REASON_MAJOR_OTHER

#endif

#include <QProcess>

#include "PowerControl.h"
#include "LocalSystem.h"

#ifdef ITALC_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef ITALC_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef ITALC_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef ITALC_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef ITALC_HAVE_ERRNO_H
#include <errno.h>
#endif

#include "ItalcConfiguration.h"
#include "PowerControl.h"
#include "Logger.h"


namespace PowerControl
{

void broadcastWOLPacket( const QString& macAddress )
{
	const int PORT_NUM = 65535;
	const int MAC_SIZE = 6;
	const int OUTBUF_SIZE = MAC_SIZE*17;
	unsigned char mac[MAC_SIZE];
	char out_buf[OUTBUF_SIZE];

	if( sscanf( macAddress.toLatin1().constData(),
				"%2x:%2x:%2x:%2x:%2x:%2x",
				(unsigned int *) &mac[0],
				(unsigned int *) &mac[1],
				(unsigned int *) &mac[2],
				(unsigned int *) &mac[3],
				(unsigned int *) &mac[4],
				(unsigned int *) &mac[5] ) != MAC_SIZE )
	{
		qWarning( "PowerControl::broadcastWOLPacket(): invalid MAC address" );
		return;
	}

	for( int i = 0; i < MAC_SIZE; ++i )
	{
		out_buf[i] = 0xff;
	}

	for( int i = 1; i < 17; ++i )
	{
		for(int j = 0; j < MAC_SIZE; ++j )
		{
			out_buf[i*MAC_SIZE+j] = mac[j];
		}
	}

#ifdef ITALC_BUILD_WIN32
	WSADATA info;
	if( WSAStartup( MAKEWORD( 2, 0 ), &info ) != 0 )
	{
		qCritical( "cannot initialize WinSock!" );
		return;
	}
#endif

	// UDP-broadcast the MAC-address
	unsigned int sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	struct sockaddr_in my_addr;
	my_addr.sin_family	  = AF_INET;			// Address family to use
	my_addr.sin_port		= htons( PORT_NUM );	// Port number to use
	my_addr.sin_addr.s_addr = inet_addr( "255.255.255.255" ); // send to
								  // IP_ADDR

	int optval = 1;
	if( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char *) &optval,
							sizeof( optval ) ) < 0 )
	{
		qCritical( "can't set sockopt (%d).", errno );
		return;
	}

	sendto( sock, out_buf, sizeof( out_buf ), 0,
			(struct sockaddr*) &my_addr, sizeof( my_addr ) );
#ifdef ITALC_BUILD_WIN32
	closesocket( sock );
	WSACleanup();
#else
	close( sock );
#endif


#if 0
#ifdef ITALC_BUILD_LINUX
	QProcess::startDetached( "etherwake " + _mac );
#endif
#endif
}



void powerDown()
{
#ifdef ITALC_BUILD_WIN32
	LocalSystem::enablePrivilege( QString::fromWCharArray( SE_SHUTDOWN_NAME ), true );
	ExitWindowsEx( EWX_POWEROFF | SHUTDOWN_FLAGS, SHUTDOWN_REASON );
	LocalSystem::enablePrivilege( QString::fromWCharArray( SE_SHUTDOWN_NAME ), false );
#else
	if( LocalSystem::User::loggedOnUser().name() == "root" )
	{
		QProcess::startDetached( "poweroff" );
	}
	else
	{
		// Gnome shutdown
		QProcess::startDetached( "dbus-send --session --dest=org.gnome.SessionManager --type=method_call /org/gnome/SessionManager org.gnome.SessionManager.RequestShutdown" );
		// KDE 3 shutdown
		QProcess::startDetached( "dcop ksmserver ksmserver logout 0 2 0" );
		// KDE 4 shutdown
		QProcess::startDetached( "qdbus org.kde.ksmserver /KSMServer logout 0 2 0" );
		// KDE 5 shutdown
		QProcess::startDetached( "dbus-send --dest=org.kde.ksmserver /KSMServer org.kde.KSMServerInterface.logout int32:0 int32:2 int32:2" );
		// generic shutdown via consolekit
		QProcess::startDetached( "dbus-send --system --dest=org.freedesktop.ConsoleKit /org/freedesktop/ConsoleKit/Manager org.freedesktop.ConsoleKit.Manager.Stop" );
	}
#endif
}



void reboot()
{
#ifdef ITALC_BUILD_WIN32
	LocalSystem::enablePrivilege( QString::fromWCharArray( SE_SHUTDOWN_NAME ), true );
	ExitWindowsEx( EWX_REBOOT | SHUTDOWN_FLAGS, SHUTDOWN_REASON );
	LocalSystem::enablePrivilege( QString::fromWCharArray( SE_SHUTDOWN_NAME ), false );
#else
	if( LocalSystem::User::loggedOnUser().name() == "root" )
	{
		QProcess::startDetached( "reboot" );
	}
	else
	{
		// Gnome reboot
		QProcess::startDetached( "dbus-send --session --dest=org.gnome.SessionManager --type=method_call /org/gnome/SessionManager org.gnome.SessionManager.RequestReboot" );
		// KDE 3 reboot
		QProcess::startDetached( "dcop ksmserver ksmserver logout 0 1 0" );
		// KDE 4 reboot
		QProcess::startDetached( "qdbus org.kde.ksmserver /KSMServer logout 0 1 0" );
		// KDE 5 reboot
		QProcess::startDetached( "dbus-send --dest=org.kde.ksmserver /KSMServer org.kde.KSMServerInterface.logout int32:1 int32:1 int32:1" );
		// generic reboot via consolekit
		QProcess::startDetached( "dbus-send --system --dest=org.freedesktop.ConsoleKit /org/freedesktop/ConsoleKit/Manager org.freedesktop.ConsoleKit.Manager.Restart" );
	}
#endif
}


} // end of namespace PowerControl

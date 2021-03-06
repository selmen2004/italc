/*
 * ItalcConfiguration.h - a Configuration object storing system wide
 *                        configuration values
 *
 * Copyright (c) 2010-2017 Tobias Doerffel <tobydox/at/users/dot/sf/dot/net>
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

#ifndef ITALC_CONFIGURATION_H
#define ITALC_CONFIGURATION_H

#include <QtCore/QStringList>

#include "ItalcCore.h"
#include "Configuration/Object.h"

#include "ItalcConfigurationProperties.h"

class ITALC_CORE_EXPORT ItalcConfiguration : public Configuration::Object
{
	Q_OBJECT
public:
	ItalcConfiguration( Configuration::Store::Backend backend =
										Configuration::Store::LocalBackend );
	ItalcConfiguration( Configuration::Store *store );
	ItalcConfiguration( const ItalcConfiguration &ref );

	static ItalcConfiguration defaultConfiguration();

	static QString expandPath( QString path );

	FOREACH_ITALC_CONFIG_PROPERTY(DECLARE_CONFIG_PROPERTY)

	// unluckily we have to declare slots manually as Qt's MOC doesn't do any
	// macro expansion :-(
public slots:
	void setApplicationName( const QString& );
	void setUiLanguage( const QString& );
	void setHighDPIScalingEnabled( bool );
	void setTrayIconHidden( bool );
	void setServiceAutostart( bool );
	void setServiceArguments( const QString & );
	void setSoftwareSASEnabled( bool );
	void setLogLevel( int );
	void setLogToStdErr( bool );
	void setLogToWindowsEventLog( bool );
	void setLimittedLogFileSize( bool );
	void setLogFileSizeLimit( int );
	void setLogFileDirectory( const QString & );
	void setNetworkObjectDirectoryPlugin( const QUuid& );
	void setNetworkObjectDirectoryUpdateInterval( int );
	void setDisabledFeatures( const QStringList& );
	void setVncServerPlugin( const QUuid& );
	void setComputerControlServerPort( int );
	void setVncServerPort( int );
	void setFeatureWorkerManagerPort( int );
	void setDemoServerPort( int );
	void setFirewallExceptionEnabled( bool );
	void setLocalConnectOnly( bool );
	void setUserConfigurationDirectory( const QString & );
	void setSnapshotDirectory( const QString & );
	void setAutoSwitchToCurrentRoom( bool );
	void setOnlyCurrentRoomVisible( bool );
	void setManualRoomAdditionAllowed( bool );
	void setLocalComputerHidden( bool );
	void setComputerDoubleClickFeature( const QUuid& );
	void setKeyAuthenticationEnabled( bool );
	void setLogonAuthenticationEnabled( bool );
	void setPermissionRequiredWithKeyAuthentication( bool );
	void setPrivateKeyBaseDir( const QString & );
	void setPublicKeyBaseDir( const QString & );
	void setPermissionRequiredWithLogonAuthentication( bool );
	void setAccessControlDataBackend( const QUuid& );
	void setAccessRestrictedToUserGroups( bool );
	void setAccessControlRulesProcessingEnabled( bool );
	void setAuthorizedUserGroups( const QStringList& );
	void setAccessControlRules( const QJsonArray& );

} ;

#endif

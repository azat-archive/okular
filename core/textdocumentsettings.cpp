/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "textdocumentsettings.h"

#include <KConfigDialog>
#include <KFontComboBox>
#include <KLocale>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSpacerItem>


using namespace Okular;

/**
 * TextDocumentSettings
 */

TextDocumentSettings::TextDocumentSettings( QWidget *parent )
    : QWidget( parent )
{
    setObjectName( QString::fromUtf8( "TextDocumentsSettingsWidget" ) );
    resize( 328, 73 );

    vboxLayout = new QVBoxLayout( this );
    vboxLayout->setObjectName( QString::fromUtf8( "vboxLayout" ) );
    vboxLayout->setContentsMargins( 0, 0, 0, 0 );

    groupBox = new QGroupBox( this );
    groupBox->setObjectName( QString::fromUtf8( "groupBox" ) );
    groupBox->setTitle( tr2i18n( "General Settings", 0 ) );
    vboxLayout->addWidget( groupBox );

    vboxLayout1 = new QVBoxLayout( groupBox );
    vboxLayout1->setObjectName( QString::fromUtf8( "vboxLayout1" ) );

    spacerItem = new QSpacerItem( 10, 5, QSizePolicy::Minimum, QSizePolicy::Expanding );
    vboxLayout->addItem( spacerItem );

    kcfg_Font = new KFontComboBox( groupBox );
    kcfg_Font->setObjectName( QString::fromUtf8( "kcfg_Font" ) );
    vboxLayout1->addWidget( kcfg_Font );

    QMetaObject::connectSlotsByName( this );
}

QFont TextDocumentSettings::font()
{
    return kcfg_Font->currentFont();
}


/**
 * TextDocumentSettingsSkeleton
 */

TextDocumentSettingsSkeleton::TextDocumentSettingsSkeleton( const QString& config, QObject *parent )
    : KConfigSkeleton( config, parent )
{
    addItemFont( "Font", mFont );
}

QFont TextDocumentSettingsSkeleton::font()
{
    return mFont;
}
/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "textdocumentsettings.h"
#include "ui_textdocumentsettings.h"

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
    Ui_TextDocumentSettings ui;
    ui.setupUi( this );

    kcfg_Font = new KFontComboBox( this );
    kcfg_Font->setObjectName( QString::fromUtf8( "kcfg_Font" ) );
    ui.formLayout->addWidget( kcfg_Font );
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
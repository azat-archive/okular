/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "textdocumentsettings.h"
#include "textdocumentsettings_p.h"
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
    , d_ptr( new TextDocumentSettingsPrivate() )
{
    Ui_TextDocumentSettings ui;
    ui.setupUi( this );

    Q_D( TextDocumentSettings );

    // TODO: we need API to add widgets
#define ADD_WIDGET( property, widget, objectName )                   \
    d->property = new widget( this );                                \
    d->property->setObjectName( QString::fromUtf8( objectName ) );   \
    ui.formLayout->addWidget( d->property );

    ADD_WIDGET( mFont, KFontComboBox, "kcfg_Font" );
#undef ADD_WIDGET
}

QFont TextDocumentSettings::font()
{
    Q_D( TextDocumentSettings );
    return d->mFont->currentFont();
}


/**
 * TextDocumentSettingsSkeleton
 */

TextDocumentSettingsSkeleton::TextDocumentSettingsSkeleton( const QString& config, QObject *parent )
    : KConfigSkeleton( config, parent )
    , d_ptr( new TextDocumentSettingsSkeletonPrivate() )
{
    Q_D( TextDocumentSettingsSkeleton );

    addItemFont( "Font", d->mFont );
}

QFont TextDocumentSettingsSkeleton::font()
{
    Q_D( TextDocumentSettingsSkeleton );
    return d->mFont;
}

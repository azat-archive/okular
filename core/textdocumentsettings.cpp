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

#include <KFontComboBox>
#include <KLocale>


using namespace Okular;

/**
 * TextDocumentSettingsWidget
 */

TextDocumentSettingsWidget::TextDocumentSettingsWidget( QWidget *parent )
    : QWidget( parent )
    , d_ptr( new TextDocumentSettingsWidgetPrivate( new Ui_TextDocumentSettings() ) )
{
    Q_D( TextDocumentSettingsWidget );

    d->mUi->setupUi( this );

    // TODO: we need API to add widgets
#define INITIALIZE_WIDGET( property, widget, objectName )            \
    d->property = new widget( this );                                \
    d->property->setObjectName( QString::fromUtf8( objectName ) );   \
    add( d->property );

    INITIALIZE_WIDGET( mFont, KFontComboBox, "kcfg_Font" );
#undef ADD_WIDGET
}

void TextDocumentSettingsWidget::add(QWidget *widget)
{
    Q_D( TextDocumentSettingsWidget );

    d->mUi->formLayout->addWidget( widget );
}

QFont TextDocumentSettingsWidget::font()
{
    Q_D( TextDocumentSettingsWidget );
    return d->mFont->currentFont();
}


/**
 * TextDocumentSettings
 */

TextDocumentSettings::TextDocumentSettings( const QString& config, QObject *parent )
    : KConfigSkeleton( config, parent )
    , d_ptr( new TextDocumentSettingsPrivate() )
{
    Q_D( TextDocumentSettings );

    addItemFont( "Font", d->mFont );
}

QFont TextDocumentSettings::font()
{
    Q_D( TextDocumentSettings );
    return d->mFont;
}

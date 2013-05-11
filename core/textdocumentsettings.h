/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef _OKULAR_TEXTDOCUMENTSETTINGS_H_
#define _OKULAR_TEXTDOCUMENTSETTINGS_H_


#include "okular_export.h"

#include <QFont>
#include <QWidget>
#include <QObject>
#include <KConfigSkeleton>

class Ui_TextDocumentSettings;
class KFontComboBox;

namespace Okular {

class TextDocumentSettingsPrivate;
class TextDocumentSettingsSkeletonPrivate;

/**
 * TextDocumentSettings
 *
 * Contain default settings for text based documents.
 * (all generators that inherited from TextDocumentGenerator)
 * Generator can add settings to this object individually.
 *
 * @since 0.16.60 (KDE 4.10)
 */
class OKULAR_EXPORT TextDocumentSettings : public QWidget
{
    public:
        TextDocumentSettings( QWidget *parent = 0 );

        QFont font();

    private:
        TextDocumentSettingsPrivate *d_ptr;
        Q_DECLARE_PRIVATE( TextDocumentSettings )
        Q_DISABLE_COPY( TextDocumentSettings )
};

/**
 * TextDocumentSettingsSkeleton
 *
 * Contain default settings/config skeleton
 * To save/restore settings.
 *
 * @since 0.16.60 (KDE 4.10)
 */
class OKULAR_EXPORT TextDocumentSettingsSkeleton : public KConfigSkeleton
{
    public:
        TextDocumentSettingsSkeleton( const QString& config, QObject *parent );

        QFont font();

    private:
        TextDocumentSettingsSkeletonPrivate *d_ptr;
        Q_DECLARE_PRIVATE( TextDocumentSettingsSkeleton )
        Q_DISABLE_COPY( TextDocumentSettingsSkeleton )
};

}

#endif

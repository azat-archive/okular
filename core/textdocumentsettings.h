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

class QVBoxLayout;
class QGroupBox;
class QSpacerItem;
class KFontComboBox;

namespace Okular {

class OKULAR_EXPORT TextDocumentSettings : public QWidget
{
    public:
        TextDocumentSettings( QWidget *parent = 0 );

        QFont font();

    private:
        QVBoxLayout *vboxLayout;
        QGroupBox *groupBox;
        QVBoxLayout *vboxLayout1;
        QSpacerItem *spacerItem;

        KFontComboBox *kcfg_Font;
};

class OKULAR_EXPORT TextDocumentSettingsSkeleton : public KConfigSkeleton
{
    public:
        TextDocumentSettingsSkeleton( const QString& config, QObject *parent );

        QFont font();

    private:
        QFont mFont;
};

}

#endif

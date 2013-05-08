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

#include <KConfigSkeleton>
#include <QWidget>

class QVBoxLayout;
class QGroupBox;
class QSpacerItem;
class KFontComboBox;

namespace Okular {

class OKULAR_EXPORT TextDocumentSettings : public QWidget
{
    public:
        TextDocumentSettings();

        QFont font();

    private:
        QVBoxLayout *vboxLayout;
        QGroupBox *groupBox;
        QVBoxLayout *vboxLayout1;
        KFontComboBox *kcfg_Font;
        QSpacerItem *spacerItem;
};

class OKULAR_EXPORT TextDocumentSettingsSkeleton : public KConfigSkeleton
{
    public:
        TextDocumentSettingsSkeleton( QString config );

        QFont font();

    private:
        QFont mFont;
};

}

#endif

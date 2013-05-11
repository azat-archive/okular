/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef _OKULAR_TEXTDOCUMENTSETTINGS_P_H_
#define _OKULAR_TEXTDOCUMENTSETTINGS_P_H_

class KFontComboBox;

namespace Okular {

class TextDocumentSettingsPrivate : public QWidget
{
    public:
        KFontComboBox *mFont;
};

class TextDocumentSettingsSkeletonPrivate : public KConfigSkeleton
{
    public:
        QFont mFont;
};

}

#endif

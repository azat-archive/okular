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
class Ui_TextDocumentSettings;

namespace Okular {

class TextDocumentSettingsWidgetPrivate
{
    public:
        TextDocumentSettingsWidgetPrivate(Ui_TextDocumentSettings *ui)
            : mUi(ui)
        {}

        KFontComboBox *mFont;
        Ui_TextDocumentSettings *mUi;
};

class TextDocumentSettingsPrivate
{
    public:
        QFont mFont;
};

}

#endif

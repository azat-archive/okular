/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _GENERATOR_TXT_H_
#define _GENERATOR_TXT_H_

#include <core/generator.h>

#include <qvector.h>

#include "ktxt.h"

namespace Okular {
class ObjectRect;
}

class TxtGenerator : public Okular::Generator
{
    Q_OBJECT
    public:
        TxtGenerator( QObject *parent, const QVariantList &args );
        ~TxtGenerator();
        bool loadDocument( const QString & fileName, QVector<Okular::Page*> & pagesVector );

        // document information
        const Okular::DocumentInfo * generateDocumentInfo();
        const Okular::DocumentSynopsis * generateDocumentSynopsis();

        // printing
        bool print( QPrinter& printer );

        QVariant metaData( const QString & key, const QVariant & option ) const;

    protected:
        bool doCloseDocument();
        // pixmap generation
        QImage image( Okular::PixmapRequest *request );
        Okular::TextPage* textPage( Okular::Page *page );

    private:
        void loadPages( QVector<Okular::Page*> & pagesVector, int rotation );
        Okular::ObjectRect* convertKTxtLink( int page, KTxt::Link * link ) const;

        KTxt *m_txt;

        Okular::DocumentInfo *m_docInfo;
        Okular::DocumentSynopsis *m_docSyn;
};

#endif

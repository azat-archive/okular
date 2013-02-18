/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include <QFile>
#include <QDataStream>
#include <QTextCodec>

#include <kencodingprober.h>
#include <kdebug.h>

#include "document.h"

using namespace Txt;

Document::Document( QString fileName )
{
#ifdef KTXT_DEBUG
    kDebug() << "Opening file" << fileName;
#endif

    QFile plainFile( fileName );
    if ( !plainFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        kDebug() << "Can't open file" << plainFile.fileName();
        return;
    }

    QByteArray buffer = plainFile.readAll();
    setPlainText( toUnicode(buffer) );
}

Document::~Document()
{
}

QByteArray Document::detectEncoding( const QByteArray &array )
{
    // TODO: see to "katetextloader.h"
    KEncodingProber prober(KEncodingProber::Universal);
    prober.feed(array);
    if (!prober.confidence() > 0.5)
    {
        kDebug() << "Can't detect charset";
        return "";
    }

#ifdef KTXT_DEBUG
    kDebug() << "Detected" << prober.encoding() << "encoding";
#endif
    return prober.encoding();
}

QString Document::toUnicode( const QByteArray &array )
{
    QByteArray encoding = detectEncoding( array );
    if ( encoding.isEmpty() )
    {
        return "";
    }
    return QTextCodec::codecForName( encoding )->toUnicode( array );
}

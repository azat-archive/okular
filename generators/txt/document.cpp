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
    : m_detectedEncoding( "" )
{
#ifdef KTXT_DEBUG
    kWarning() << "Opening file" << fileName;
#endif

    QFile plainFile( fileName );
    if ( !plainFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        kWarning() << "Can't open file" << plainFile.fileName();
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
    if ( !m_detectedEncoding.isEmpty() )
        return m_detectedEncoding;

    // TODO: see to "katetextloader.h"
    KEncodingProber prober(KEncodingProber::Universal);
    prober.feed(array);
    if (!prober.confidence() > 0.5)
    {
        kWarning() << "Can't detect charsert";
        return "";
    }

#ifdef KTXT_DEBUG
    kWarning() << "Detected" << prober.encoding() << "encoding"; 
#endif
    return ( m_detectedEncoding = prober.encoding() );
}

QString Document::toUnicode( const QByteArray &array )
{
    return QTextCodec::codecForName( detectEncoding( array ) )->toUnicode( array );
}
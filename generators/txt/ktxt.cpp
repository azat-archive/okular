/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              * 
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktxt.h"

#include <qbytearray.h>
#include <qdom.h>
#include <qfile.h>
#include <qhash.h>
#include <qlist.h>
#include <qpainter.h>
#include <qqueue.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qtextcodec.h>
#include <qmath.h>


#include <kdebug.h>
#include <klocale.h>
#include <kencodingprober.h>


// ImageCacheItem

class ImageCacheItem
{
    public:
        ImageCacheItem( int p, int w, int h, const QImage& i )
          : page( p ), width( w ), height( h ), img( i ) { }

        int page;
        int width;
        int height;
        QImage img;
};


// KTxt::Page

KTxt::Page::Page( const QString& content )
    : m_width( 1080 ) // TODO: get window width / 2, and handle window resize
    , m_height ( 1080 ) // TODO: get window height, and handle window resize
    , m_content( content )
{
}

KTxt::Page::~Page()
{
}

int KTxt::Page::width() const
{
    return m_width;
}

int KTxt::Page::height() const
{
    return m_height;
}

int KTxt::Page::dpi() const
{
    return m_dpi;
}

int KTxt::Page::orientation() const
{
    return m_orientation;
}

QString KTxt::Page::content() const
{
    return m_content;
}

QImage KTxt::Page::generateImageTile( int width, int xdelta, int height, int ydelta ) const
{
    QRect renderRect(0, 0, qMin( width, xdelta ), qMin( height, ydelta ) );

    // TODO: replace kWarning() by kDebug()
#ifdef KTXT_DEBUG
    kWarning()  << "width:" << width << ", xdelta:" << xdelta
                << ", height:" << height << ", ydelta:" << ydelta;
    kWarning() << "renderRect:" << renderRect;
#endif

    QImage resImg( renderRect.width(), renderRect.height(), QImage::Format_RGB32 );
    resImg.fill( Qt::white );
    QPainter paintTo;
    paintTo.begin( &resImg );
    paintTo.drawText( 0, 0, resImg.width(), resImg.height(), Qt::TextWordWrap, content() );
    paintTo.end();

    return resImg;
}

// KTxt::Link

KTxt::Link::~Link()
{
}

KTxt::Link::LinkArea KTxt::Link::areaType() const
{
    return m_area;
}

QPoint KTxt::Link::point() const
{
    return m_point;
}

QSize KTxt::Link::size() const
{
    return m_size;
}

QPolygon KTxt::Link::polygon() const
{
    return m_poly;
}

// KTxt::PageLink

KTxt::PageLink::PageLink()
{
}

int KTxt::PageLink::type() const
{
    return KTxt::Link::PageLink;
}

QString KTxt::PageLink::page() const
{
    return m_page;
}

// KTxt::UrlLink

KTxt::UrlLink::UrlLink()
{
}

int KTxt::UrlLink::type() const
{
    return KTxt::Link::UrlLink;
}

QString KTxt::UrlLink::url() const
{
    return m_url;
}

// KTxt::TextEntity

KTxt::TextEntity::TextEntity()
{
}

KTxt::TextEntity::~TextEntity()
{
}

QString KTxt::TextEntity::text() const
{
    return m_text;
}

QRect KTxt::TextEntity::rect() const
{
    return m_rect;
}

// TODO: split to files
// TODO: support of utf8
// TODO: custom charset
// TODO: try to map
class KTxt::Document
{
    public:
        Document( QString fileName )
            : m_file( new QFile(fileName) )
            , m_fileStream( NULL )
            , m_detectedEncoding( "" )
        {
#ifdef KTXT_DEBUG
            kWarning() << "Opening file" << fileName;
#endif

            // TODO
            if ( !m_file->open( QIODevice::ReadOnly | QIODevice::Text) )
                kWarning() << "Can't open file" << fileName;
            m_fileStream = new QDataStream( m_file );
        }

        ~Document()
        {
            if (m_fileStream)
                delete m_fileStream;
            delete m_file;
        }

        QString at( int page );
        int pages();

    private:
        // TODO: portability to old compilers
        // TODO: if we already have file in unicode, increase double CHARS_PER_PAGE const
        // TODO: generate CHARS_PER_PAGE on the basis of font
        static const int CHARS_PER_PAGE = 6000;

        QFile *m_file;
        QDataStream *m_fileStream;
        QByteArray m_detectedEncoding;

        char buffer[ CHARS_PER_PAGE ];

        // TODO: write a better detecter, based on some number of chunks
        QByteArray detectEncoding( const QByteArray &array );
        QString toUnicode( const QByteArray &array );
};

int KTxt::Document::pages()
{
    return qCeil( (float)m_file->size() / CHARS_PER_PAGE );
}

QString KTxt::Document::at( int page )
{
    // TODO: check is such page exist
    Q_ASSERT( m_fileStream->seek( page * CHARS_PER_PAGE ) );

    memset( buffer, 0, CHARS_PER_PAGE );
    int bytesRead = m_fileStream->readRawData( buffer, CHARS_PER_PAGE );
    if ( bytesRead == -1 )
    {
        kWarning() << "Can't read" << page << "page";
        return "";
    }

#ifdef KTXT_DEBUG
    kWarning() << "bytesRead" << bytesRead;
#endif
    return toUnicode( QByteArray(  buffer, bytesRead ) );
}

QByteArray KTxt::Document::detectEncoding( const QByteArray &array )
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

    kWarning() << "Detected" << prober.encoding() << "encoding"; 
    return ( m_detectedEncoding = prober.encoding() );
}

QString KTxt::Document::toUnicode( const QByteArray &array )
{
    QString unicoded = QTextCodec::codecForName( detectEncoding( array ) )->toUnicode( array );

#ifdef KTXT_DEBUG
    kWarning() << "bytesRead" << unicoded.length();
#endif
    return unicoded;
}

class KTxt::Private
{
    public:
        Private()
          : m_txtDocument( NULL )
          , m_docBookmarks( 0 )
          , m_cacheEnabled( true )
        {
        }

        void readBookmarks();

        void readMetaData( int page );

        KTxt::Document *m_txtDocument;

        QVector<KTxt::Page*> m_pages;

        QList<ImageCacheItem*> mImgCache;

        QHash<QString, QVariant> m_metaData;
        QDomDocument * m_docBookmarks;

        QHash<QString, int> m_pageNamesCache;

        bool m_cacheEnabled;
};

// TODO
void KTxt::Private::readBookmarks()
{
}

// TODO
void KTxt::Private::readMetaData( int page )
{
    if ( !m_txtDocument )
        return;
}

KTxt::KTxt()
    : d( new Private )
{
}

KTxt::~KTxt()
{
    closeFile();

    delete d;
}

bool KTxt::openFile( const QString & fileName )
{
    // first, close the old file
    if ( d->m_txtDocument )
        closeFile();

    // load the document...
    d->m_txtDocument = new KTxt::Document( fileName );

    int numOfPages = d->m_txtDocument->pages();
    kDebug() << "# of pages:" << numOfPages;
    d->m_pages.clear();
    d->m_pages.resize( numOfPages );

    d->m_metaData[ "documentType" ] = i18nc( "Type of Txt document", "PlainText" );

    // read the pages
    for ( int i = 0; i < numOfPages; ++i )
        d->m_pages[i] = new KTxt::Page( d->m_txtDocument->at( i ) );

    // reading the metadata from the first page only should be enough
    if ( numOfPages > 0 )
        d->readMetaData( 0 );

    return true;
}

void KTxt::closeFile()
{
    // deleting the old TOC
    delete d->m_docBookmarks;
    d->m_docBookmarks = NULL;
    // deleting the pages
    qDeleteAll( d->m_pages );
    d->m_pages.clear();
    // clearing the image cache
    qDeleteAll( d->mImgCache );
    d->mImgCache.clear();
    // clearing the old metadata
    d->m_metaData.clear();
    // cleaing the page names mapping
    d->m_pageNamesCache.clear();
    // releasing the old document
    if ( d->m_txtDocument )
        delete d->m_txtDocument;
    d->m_txtDocument = NULL;
}

QVariant KTxt::metaData( const QString & key ) const
{
    QHash<QString, QVariant>::ConstIterator it = d->m_metaData.constFind( key );
    return it != d->m_metaData.constEnd() ? it.value() : QVariant();
}

const QDomDocument * KTxt::documentBookmarks() const
{
    if ( !d->m_docBookmarks )
        d->readBookmarks();
    return d->m_docBookmarks;
}

const QVector<KTxt::Page*> &KTxt::pages() const
{
    return d->m_pages;
}

QImage KTxt::image( int page, int width, int height, int rotation )
{
    if ( d->m_cacheEnabled )
    {
        bool found = false;
        QList<ImageCacheItem*>::Iterator it = d->mImgCache.begin(), itEnd = d->mImgCache.end();
        for ( ; ( it != itEnd ) && !found; ++it )
        {
            ImageCacheItem* cur = *it;
            if ( ( cur->page == page ) &&
                 ( rotation % 2 == 0
                   ? cur->width == width && cur->height == height
                   : cur->width == height && cur->height == width ) )
                found = true;
        }
        if ( found )
        {
            // taking the element and pushing to the top of the list
            --it;
            ImageCacheItem* cur2 = *it;
            d->mImgCache.erase( it );
            d->mImgCache.push_front( cur2 );

            return cur2->img;
        }
    }

    KTxt::Page *txtPage = d->m_pages[page];

    static const int xdelta = 1500;
    static const int ydelta = 1500;

    QImage newimg = txtPage->generateImageTile( width, xdelta, height, ydelta );

    if ( d->m_cacheEnabled )
    {
        // delete all the cached pixmaps for the current page with a size that
        // differs no more than 35% of the new pixmap size
        int imgsize = newimg.width() * newimg.height();
        if ( imgsize > 0 )
        {
            for( int i = 0; i < d->mImgCache.count(); )
            {
                ImageCacheItem* cur = d->mImgCache.at(i);
                if ( ( cur->page == page ) &&
                     ( abs( cur->img.width() * cur->img.height() - imgsize ) < imgsize * 0.35 ) )
                {
                    d->mImgCache.removeAt( i );
                    delete cur;
                }
                else
                    ++i;
            }
        }

        // the image cache has too many elements, remove the last
        if ( d->mImgCache.size() >= 10 )
        {
            delete d->mImgCache.last();
            d->mImgCache.removeLast();
        }
        ImageCacheItem* ich = new ImageCacheItem( page, width, height, newimg );
        d->mImgCache.push_front( ich );
    }

    return newimg;
}

bool KTxt::exportAsPostScript( const QString & fileName, const QList<int>& pageList ) const
{
    if ( fileName.trimmed().isEmpty() || pageList.isEmpty() )
        return false;

    QFile f( fileName );
    f.open( QIODevice::ReadWrite );
    bool ret = exportAsPostScript( &f, pageList );
    if ( ret )
    {
        f.close();
    }
    return ret;
}

bool KTxt::exportAsPostScript( QFile* file, const QList<int>& pageList ) const
{
    if ( !file || pageList.isEmpty() )
        return false;

    QTextStream out(file);

    QString pl;
    foreach ( int p, pageList )
    {
        out << d->m_txtDocument->at(p);
    }

    return true;
}

// TODO
QList<KTxt::TextEntity> KTxt::textEntities( int page, const QString & granularity ) const
{
    QList<KTxt::TextEntity> ret;
    return ret;
}

void KTxt::setCacheEnabled( bool enable )
{
    if ( enable == d->m_cacheEnabled )
        return;

    d->m_cacheEnabled = enable;
    if ( !d->m_cacheEnabled )
    {
        qDeleteAll( d->mImgCache );
        d->mImgCache.clear();
    }
}

bool KTxt::isCacheEnabled() const
{
    return d->m_cacheEnabled;
}


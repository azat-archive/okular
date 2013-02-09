/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "generator_txt.h"

#include <core/action.h>
#include <core/annotations.h>
#include <core/area.h>
#include <core/document.h>
#include <core/page.h>
#include <core/textpage.h>
#include <core/utils.h>
#include <core/fileprinter.h>

#include <qdom.h>
#include <qmutex.h>
#include <qpixmap.h>
#include <qstring.h>
#include <quuid.h>
#include <QtGui/QPrinter>

#include <kaboutdata.h>
#include <kdebug.h>
#include <klocale.h>
#include <ktemporaryfile.h>


static KAboutData createAboutData()
{
    KAboutData aboutData(
         "okular_txt",
         "okular_txt",
         ki18n( "Txt Backend" ),
         "0.1",
         ki18n( "Txt backend." ),
         KAboutData::License_GPL,
         ki18n( "© 2013 Azat Khuzhin" )
    );
    aboutData.addAuthor( ki18n( "Azat Khuzhin" ), KLocalizedString(), "a3at.mail@gmail.com" );
    return aboutData;
}

OKULAR_EXPORT_PLUGIN( TxtGenerator, createAboutData() )

TxtGenerator::TxtGenerator( QObject *parent, const QVariantList &args )
    : Okular::Generator( parent, args ), m_docInfo( 0 ), m_docSyn( 0 )
{
    setFeature( TextExtraction );
    setFeature( Threaded );
    setFeature( PrintPostscript );
    if ( Okular::FilePrinter::ps2pdfAvailable() )
        setFeature( PrintToFile );

    m_txt = new KTxt();
    m_txt->setCacheEnabled( false );
}

TxtGenerator::~TxtGenerator()
{
    delete m_txt;
}

bool TxtGenerator::loadDocument( const QString & fileName, QVector< Okular::Page * > & pagesVector )
{
    QMutexLocker locker( userMutex() );
    if ( !m_txt->openFile( fileName ) )
        return false;

    locker.unlock();

    loadPages( pagesVector, 0 );

    return true;
}

bool TxtGenerator::doCloseDocument()
{
    userMutex()->lock();
    m_txt->closeFile();
    userMutex()->unlock();

    delete m_docInfo;
    m_docInfo = 0;
    delete m_docSyn;
    m_docSyn = 0;

    return true;
}

QImage TxtGenerator::image( Okular::PixmapRequest *request )
{
    userMutex()->lock();
    QImage img = m_txt->image( request->pageNumber(), request->width(), request->height(), request->page()->rotation() );
    userMutex()->unlock();
    return img;
}

const Okular::DocumentInfo * TxtGenerator::generateDocumentInfo()
{
    if ( m_docInfo )
        return m_docInfo;

    m_docInfo = new Okular::DocumentInfo();

    m_docInfo->set( Okular::DocumentInfo::MimeType, "plain/text" );

    return m_docInfo;
}

// TODO
const Okular::DocumentSynopsis * TxtGenerator::generateDocumentSynopsis()
{
    QMutexLocker locker( userMutex() );
    if ( m_docSyn )
        return m_docSyn;

    const QDomDocument *doc = m_txt->documentBookmarks();
    if ( doc )
    {
        m_docSyn = new Okular::DocumentSynopsis();
    }
    locker.unlock();

    return m_docSyn;
}

bool TxtGenerator::print( QPrinter& printer )
{
    bool result = false;

    // Create tempfile to write to
    KTemporaryFile tf;
    tf.setSuffix( ".ps" );
    if ( !tf.open() )
        return false;

    QMutexLocker locker( userMutex() );
    QList<int> pageList = Okular::FilePrinter::pageList( printer, m_txt->pages().count(),
                                                         document()->currentPage() + 1,
                                                         document()->bookmarkedPageList() );

    if ( m_txt->exportAsPostScript( &tf, pageList ) )
    {
        tf.setAutoRemove( false );
        const QString fileName = tf.fileName();
        tf.close();
        int ret = Okular::FilePrinter::printFile( printer, fileName, document()->orientation(),
                                                  Okular::FilePrinter::SystemDeletesFiles,
                                                  Okular::FilePrinter::ApplicationSelectsPages,
                                                  document()->bookmarkedPageRange() );
        result = ( ret >=0 );
    }

    return result;
}

QVariant TxtGenerator::metaData( const QString &key, const QVariant &option ) const
{
    Q_UNUSED( option )
    if ( key == "DocumentTitle" )
    {
        return m_txt->metaData( "title" );
    }
    return QVariant();
}

Okular::TextPage* TxtGenerator::textPage( Okular::Page *page )
{
    userMutex()->lock();
    QList<KTxt::TextEntity> te;
    if ( te.isEmpty() )
        te = m_txt->textEntities( page->number(), "word" );
    if ( te.isEmpty() )
        te = m_txt->textEntities( page->number(), "line" );
    userMutex()->unlock();
    QList<KTxt::TextEntity>::ConstIterator it = te.constBegin();
    QList<KTxt::TextEntity>::ConstIterator itEnd = te.constEnd();
    QList<Okular::TextEntity*> words;
    const KTxt::Page* txtPage = m_txt->pages().at( page->number() );
    for ( ; it != itEnd; ++it )
    {
        const KTxt::TextEntity& cur = *it;
        words.append( new Okular::TextEntity( cur.text(), new Okular::NormalizedRect( cur.rect(), txtPage->width(), txtPage->height() ) ) );
    }
    Okular::TextPage *textpage = new Okular::TextPage( words );
    return textpage;
}


void TxtGenerator::loadPages( QVector<Okular::Page*> & pagesVector, int rotation )
{
    const QVector<KTxt::Page*> &txtPages = m_txt->pages();
    int numofpages = txtPages.count();
    pagesVector.resize( numofpages );

    for ( int i = 0; i < numofpages; ++i )
    {
        const KTxt::Page *p = txtPages.at( i );
        if (pagesVector[i])
            delete pagesVector[i];
        int w = p->width();
        int h = p->height();
        if ( rotation % 2 == 1 )
            qSwap( w, h );
        Okular::Page *page = new Okular::Page( i, w, h, (Okular::Rotation)( p->orientation() + rotation ) );
        pagesVector[i] = page;

        // TODO: links & annotation
    }
}

Okular::ObjectRect* TxtGenerator::convertKTxtLink( int page, KTxt::Link * link ) const
{
    int newpage = -1;
    Okular::Action *newlink = 0;
    Okular::ObjectRect *newrect = 0;
    switch ( link->type() )
    {
        case KTxt::Link::PageLink:
        {
            KTxt::PageLink* l = static_cast<KTxt::PageLink*>( link );
            bool ok = true;
            QString target = l->page();
            if ( ( target.length() > 0 ) && target.at(0) == QLatin1Char( '#' ) )
                target.remove( 0, 1 );
            int tmppage = target.toInt( &ok );
            if ( ok || target.isEmpty() )
            {
                Okular::DocumentViewport vp;
                if ( !target.isEmpty() )
                {
                    vp.pageNumber = ( target.at(0) == QLatin1Char( '+' ) || target.at(0) == QLatin1Char( '-' ) ) ? page + tmppage : tmppage - 1;
                    newpage = vp.pageNumber;
                }
                newlink = new Okular::GotoAction( QString(), vp );
            }
            break;
        }
        case KTxt::Link::UrlLink:
        {
            KTxt::UrlLink* l = static_cast<KTxt::UrlLink*>( link );
            QString url = l->url();
            newlink = new Okular::BrowseAction( url );
            break;
        }
    }
    if ( newlink )
    {
        const KTxt::Page* p = m_txt->pages().value( newpage == -1 ? page : newpage );
        if ( !p )
            p = m_txt->pages().at( page );
        int width = p->width();
        int height = p->height();
        bool scape_orientation = false; // hack by tokoe, should always create default page
        if ( scape_orientation )
            qSwap( width, height );
        switch ( link->areaType() )
        {
            case KTxt::Link::RectArea:
            case KTxt::Link::EllipseArea:
            {
                QRect r( QPoint( link->point().x(), p->height() - link->point().y() - link->size().height() ), link->size() );
                bool ellipse = ( link->areaType() == KTxt::Link::EllipseArea );
                newrect = new Okular::ObjectRect( Okular::NormalizedRect( Okular::Utils::rotateRect( r, width, height, 0 ), width, height ), ellipse, Okular::ObjectRect::Action, newlink );
                break;
            }
            case KTxt::Link::PolygonArea:
            {
                QPolygon poly = link->polygon();
                QPolygonF newpoly;
                for ( int i = 0; i < poly.count(); ++i )
                {
                    int x = poly.at(i).x();
                    int y = poly.at(i).y();
                    if ( scape_orientation )
                        qSwap( x, y );
                    else
                    {
                        y = height - y;
                    }
                    newpoly << QPointF( (double)(x)/width, (double)(y)/height );
                }
                if ( !newpoly.isEmpty() )
                {
                    newpoly << newpoly.first();
                    newrect = new Okular::ObjectRect( newpoly, Okular::ObjectRect::Action, newlink );
                }
                break;
            }
            default: ;
        }
        if ( !newrect )
        {
            delete newlink;
        }
    }
    return newrect;
}


#include "generator_txt.moc"

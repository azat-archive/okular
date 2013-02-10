/***************************************************************************
 *   Copyright (C) 2013 by Azat Khuzhin <a3at.mail@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _KTXT_
#define _KTXT_

#include <qcolor.h>
#include <qimage.h>
#include <qlist.h>
#include <qpolygon.h>
#include <qrect.h>
#include <qvariant.h>
#include <qvector.h>

class QDomDocument;
class QFile;

class KTxt
{
    public:
        KTxt();
        ~KTxt();

        /**
         * A Txt page.
         */
        class Page
        {
            friend class KTxt;

            public:
                ~Page();

                int width() const;
                int height() const;
                int dpi() const;
                int orientation() const;
                QString content() const;
                QImage generateImageTile( int width, int xdelta, int height, int ydelta ) const;

            private:
                Page( const QString& content );

                int m_width;
                int m_height;
                int m_dpi;
                int m_orientation;

                QString m_content;
        };


        /**
         * The base implementation for a Txt link.
         */
        class Link
        {
            friend class KTxt;

            public:
                virtual ~Link();

                enum LinkType { PageLink, UrlLink };
                enum LinkArea { UnknownArea, RectArea, EllipseArea, PolygonArea };
                virtual int type() const = 0;
                LinkArea areaType() const;
                QPoint point() const;
                QSize size() const;
                QPolygon polygon() const;

            private:
                LinkArea m_area;
                QPoint m_point;
                QSize m_size;
                QPolygon m_poly;
        };

        /**
         * A link to reach a page of a Txt document.
         */
        class PageLink : public Link
        {
            friend class KTxt;

            public:
                virtual int type() const;
                QString page() const;

            private:
                PageLink();
                QString m_page;
        };

        /**
         * A Txt link to open an external Url.
         */
        class UrlLink : public Link
        {
            friend class KTxt;

            public:
                virtual int type() const;
                QString url() const;

            private:
                UrlLink();
                QString m_url;
        };

        /**
         * A Txt text entity.
         */
        class TextEntity
        {
            friend class KTxt;

            public:
                ~TextEntity();

                QString text() const;
                QRect rect() const;

            private:
                TextEntity();

                QString m_text;
                QRect m_rect;
        };

        /**
         * Opens the file \p fileName, closing the old one if necessary.
         */
        bool openFile( const QString & fileName );
        /**
         * Close the file currently opened, if any.
         */
        void closeFile();

        /**
         * The pages of the current document, or an empty vector otherwise.
         * \note KTxt handles the pages, so you don't need to delete them manually
         * \return a vector with the pages of the current document
         */
        const QVector<KTxt::Page*> &pages() const;

        /**
         * Get the metadata for the specified \p key, or a null variant otherwise.
         */
        QVariant metaData( const QString & key ) const;

        /**
         * Get an XML document with the bookmarks of the current document (if any).
         * The XML will look like this:
         * \verbatim
         * <!DOCTYPE KTxtBookmarks>
         * <item title="Title 1" destination="dest1">
         *   <item title="Title 1.1" destination="dest1.1" />
         *   ...
         * </item>
         * <item title="Title 2" destination="dest2">
         * \endverbatim
         */
        const QDomDocument * documentBookmarks() const;

        /**
         * Check if the image for the specified \p page with the specified
         * \p width, \p height and \p rotation is already in cache, and returns
         * it. If not, a null image is returned.
         */
        QImage image( int page, int width, int height, int rotation );

        /**
         * Export the currently open document as PostScript file \p fileName.
         * \returns whether the exporting was successful
          */
        bool exportAsPostScript( const QString & fileName, const QList<int>& pageList ) const;

        /**
         * Export the currently open document as PostScript file.
         * \returns whether the exporting was successful
          */
        bool exportAsPostScript( QFile* file, const QList<int>& pageList ) const;

        /**
         * Return the list of the text entities for the specified \p page, that matches the
         * specified \p granularity.
         */
        QList<KTxt::TextEntity> textEntities( int page, const QString & granularity ) const;

        /**
         * Enable or disable the internal rendered pages cache.
         */
        void setCacheEnabled( bool enable );
        /**
         * \returns whether the internal rendered pages cache is enabled
         */
        bool isCacheEnabled() const;

    private:
        class Private;
        Private * const d;

        class Document;
};

#endif

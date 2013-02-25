/***************************************************************************
 *   Copyright (C) 2007 by Tobias Koenig <tokoe@kde.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _OKULAR_TEXTDOCUMENTGENERATOR_H_
#define _OKULAR_TEXTDOCUMENTGENERATOR_H_

#include <QFont>

#include <interfaces/configinterface.h>

#include "okular_export.h"

#include "document.h"
#include "generator.h"

class QTextBlock;
class QTextDocument;

namespace Okular {

class TextDocumentConverterPrivate;
class TextDocumentGenerator;
class TextDocumentGeneratorPrivate;

class OKULAR_EXPORT TextDocumentConverter : public QObject
{
    Q_OBJECT

    friend class TextDocumentGenerator;
    friend class TextDocumentGeneratorPrivate;

    public:
        /**
         * Creates a new generic converter.
         */
        TextDocumentConverter();

        /**
         * Destroys the generic converter.
         */
        ~TextDocumentConverter();

        /**
         * Returns the generated QTextDocument object.
         */
        virtual QTextDocument *convert( const QString &fileName ) = 0;

    Q_SIGNALS:
        /**
         * Adds a new link object which is located between cursorBegin and
         * cursorEnd to the generator.
         */
        void addAction( Action *link, int cursorBegin, int cursorEnd );

        /**
         * Adds a new annotation object which is located between cursorBegin and
         * cursorEnd to the generator.
         */
        void addAnnotation( Annotation *annotation, int cursorBegin, int cursorEnd );

        /**
         * Adds a new title at the given level which is located as position to the generator.
         */
        void addTitle( int level, const QString &title, const QTextBlock &position );

        /**
         * Adds a set of meta data to the generator.
         */
        void addMetaData( const QString &key, const QString &value, const QString &title );

        /**
         * Adds a set of meta data to the generator.
         *
         * @since 0.7 (KDE 4.1)
         */
        void addMetaData( DocumentInfo::Key key, const QString &value );

        /**
         * This signal should be emitted whenever an error occurred in the converter.
         *
         * @param message The message which should be shown to the user.
         * @param duration The time that the message should be shown to the user.
         */
        void error( const QString &message, int duration );

        /**
         * This signal should be emitted whenever the user should be warned.
         *
         * @param message The message which should be shown to the user.
         * @param duration The time that the message should be shown to the user.
         */
        void warning( const QString &message, int duration );

        /**
         * This signal should be emitted whenever the user should be noticed.
         *
         * @param message The message which should be shown to the user.
         * @param duration The time that the message should be shown to the user.
         */
        void notice( const QString &message, int duration );

    protected:
        /**
         * This method can be used to calculate the viewport for a given text block.
         *
         * @note This method should be called at the end of the convertion, because it
         *       triggers QTextDocument to do the layout calculation.
         */
        DocumentViewport calculateViewport( QTextDocument *document, const QTextBlock &block );

        /**
         * Returns the generator that owns this converter.
         *
         * @note May be null if the converter was not created for a generator.
         *
         * @since 0.7 (KDE 4.1)
         */
        TextDocumentGenerator* generator() const;

    private:
        TextDocumentConverterPrivate *d_ptr;
        Q_DECLARE_PRIVATE( TextDocumentConverter )
        Q_DISABLE_COPY( TextDocumentConverter )
};

/**
 * @brief QTextDocument-based Generator
 *
 * This generator provides a document in the form of a QTextDocument object,
 * parsed using a specialized TextDocumentConverter.
 */
class OKULAR_EXPORT TextDocumentGenerator : public Generator, public Okular::ConfigInterface
{
    /// @cond PRIVATE
    friend class TextDocumentConverter;
    /// @endcond

    Q_OBJECT
    Q_INTERFACES( Okular::ConfigInterface )

    public:
        /**
         * Creates a new generator that uses the specified @p converter.
         *
         * @note the generator will take ownership of the converter, so you
         *       don't have to delete it yourself
         */
        TextDocumentGenerator( TextDocumentConverter *converter, QObject *parent, const QVariantList &args );
        virtual ~TextDocumentGenerator();

        // [INHERITED] load a document and fill up the pagesVector
        bool loadDocument( const QString & fileName, QVector<Okular::Page*> & pagesVector );

        // [INHERITED] perform actions on document / pages
        bool canGeneratePixmap() const;
        void generatePixmap( Okular::PixmapRequest * request );

        // [INHERITED] print document using already configured QPrinter
        bool print( QPrinter& printer );

        // [INHERITED] text exporting
        Okular::ExportFormat::List exportFormats() const;
        bool exportTo( const QString &fileName, const Okular::ExportFormat &format );

        // [INHERITED] reparse configuration
        bool reparseConfig();
        void addPages( KConfigDialog* dlg );

        const Okular::DocumentInfo* generateDocumentInfo();
        const Okular::DocumentSynopsis* generateDocumentSynopsis();

    protected:
        bool doCloseDocument();
        Okular::TextPage* textPage( Okular::Page *page );

    private:
        Q_DECLARE_PRIVATE( TextDocumentGenerator )
        Q_DISABLE_COPY( TextDocumentGenerator )

        Q_PRIVATE_SLOT( d_func(), void addAction( Action*, int, int ) )
        Q_PRIVATE_SLOT( d_func(), void addAnnotation( Annotation*, int, int ) )
        Q_PRIVATE_SLOT( d_func(), void addTitle( int, const QString&, const QTextBlock& ) )
        Q_PRIVATE_SLOT( d_func(), void addMetaData( const QString&, const QString&, const QString& ) )
        Q_PRIVATE_SLOT( d_func(), void addMetaData( DocumentInfo::Key, const QString& ) )
};

}

#endif

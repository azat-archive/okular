/***************************************************************************
 *   Copyright (C) 2008 by Ely Levy <elylevy@cs.huji.ac.il>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QColor>

#include "generator_epub.h"

#include "converter.h"

#include <kaboutdata.h>

static KAboutData createAboutData()
{
  KAboutData aboutData(
                       "okular_epub",
                       "okular_epub",
                       ki18n("EPub Backend"),
                       "0.2.1",
                       ki18n("An EPub backend"),
                       KAboutData::License_GPL,
                       ki18n("© 2008 Ely Levy")
                       );
  aboutData.addAuthor(ki18n("Ely Levy"), KLocalizedString(),
                      "elylevy@cs.huji.ac.il");

  return aboutData;
}

OKULAR_EXPORT_PLUGIN( EPubGenerator, createAboutData() )

EPubGenerator::EPubGenerator( QObject *parent, const QVariantList &args )
: Okular::TextDocumentGenerator( new Epub::Converter, parent, args )
{
}

const QColor& EPubGenerator::background()
{
  const Epub::Converter *epubConverter = qobject_cast< const Epub::Converter* >( converter() );
  kWarning() << epubConverter->background().color();
  return epubConverter->background().color();
}

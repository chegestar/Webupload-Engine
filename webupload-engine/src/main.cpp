 
/*
 * Web Upload Engine -- MeeGo social networking uploads
 * Copyright (c) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Jukka Tiihonen <jukka.t.tiihonen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <QDebug>
#include <QCoreApplication>
#include <MLocale>
#include "WebUpload/System"
#include "WebUpload/Error"
#include "logger.h"
#include "uploadengineadaptor.h"
#include "uploadengine.h"

Q_DECL_EXPORT int main(int argc, char **argv) {

    QCoreApplication::setApplicationName (QLatin1String ("WebUpload Engine"));
    QCoreApplication::setOrganizationName (QLatin1String ("MeeGo"));

    // Let's use custom logging
    Logger * logger = Logger::newIfEnabled ();
    
    DBGSTREAM << "Web upload engine started";

    WebUpload::System::registerMetaTypes ();
    
    //TODO: This is workaround to avoid catalog loading in 3rd thread
    WebUpload::System::setLoadPresentationDataEnabled (false);

    UploadEngine * engine = new UploadEngine(argc, argv);
    new UploadEngineAdaptor (engine);

    // Register DBUS interface
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.registerService("com.meego.sharing.webuploadengine")) {
        qCritical() << "DBUS service already taken";
        delete engine;

        return EXIT_FAILURE;
    }

    qDebug() << " register object" << connection.registerObject("/", engine);

    MLocale locale;
    // Seems like translation path needs to be added translation to work with
    // non-meegotouch apps
    MLocale::addTranslationPath ("/usr/share/l10n/meegotouch");
    // Install engineering english application catalog 
    locale.installTrCatalog ("webupload-engine"); 
    MLocale::setDefault (locale);

    WebUpload::System::loadLocales();

    int ret = engine->exec();
    
    DBGSTREAM << "Destroy engine";
    delete engine;
    
    DBGSTREAM << "Clean web upload engine shutdown" << ret;
    
    delete logger;    
    return ret;
}

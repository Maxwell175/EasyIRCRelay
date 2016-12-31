/*
 * EasyIRCRelay
 * Copyright 2016 (c) - Maxwell Dreytser
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QObject>
#include <QSet>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSocketNotifier>

#include <Irc>
#include <IrcConnection>
#include <IrcCommand>
#include <IrcCommandParser>
#include <ircdebug_p.h>

#include "sigwatch.h"

class MainWorker : public QObject
{

public:
    void StartMainWorker();
    void StartMainWorker(QString myConfigPath);

    QSet<QVariantMap> RelayNetworks;

    void ReloadConfig();

private:

    QString configPath = "config.json";

    const QString BoldCode = "\x02";
    const QString ColorCode = "\x03";
    const QString ItalicCode = "\x1D";
    const QString UnderlineCode = "\x1F";

    QString GlobalTopic = "";

    QSet<QVariantMap> ParseConfig();

    bool containsNetwork(QSet<QVariantMap> A, QVariantMap B);

    void processMessage(IrcPrivateMessage* message);
    void processNickChange(IrcNickMessage* message);
    void processJoin(IrcJoinMessage* message);
    void processPart(IrcPartMessage* message);
    void processQuit(IrcQuitMessage* message);

    QString FormatTopic(QString Format);
    QString FormatMessage(QString Format, QString Nick, QString Message);
    QString FormatAction(QString Format, QString Nick, QString Action);
    QString FormatNickChangeMessage(QString Format, QString OldNick, QString NewNick);
    QString FormatJoinMessage(QString Format, QString Nick, QString Ident, QString Host);
    QString FormatPartMessage(QString Format, QString Nick, QString Ident, QString Host);
    QString FormatQuitMessage(QString Format, QString Nick, QString Ident, QString Host, QString QuitMsg);


    UnixSignalWatcher sigwatcher;

    void unixSignalHandler(int signal);
};

inline uint qHash(const QVariantMap &map, uint seed) {
    return qHash(QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact), seed);
}

#endif // MAINWORKER_H

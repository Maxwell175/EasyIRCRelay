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


#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegExp>
#include <QTimer>

#include "mainworker.h"

void MainWorker::StartMainWorker()
{
    ReloadConfig();

    sigwatcher.watchForSignal(SIGHUP);
    sigwatcher.watchForSignal(SIGTERM);
    connect(&sigwatcher, &UnixSignalWatcher::unixSignal, this, &MainWorker::unixSignalHandler);
}

void MainWorker::StartMainWorker(QString myConfigPath) {
    configPath = myConfigPath;

    StartMainWorker();
}

void MainWorker::unixSignalHandler(int signal) {
    if (signal == SIGHUP) {
        ReloadConfig();
    } else if (signal == SIGTERM) {
        foreach (QVariantMap RelayedNetwork, RelayNetworks) {
            IrcConnection* CurrConn = (IrcConnection*) RelayedNetwork["Connection"].value<QObject*>();
            CurrConn->quit();
            CurrConn->close();
        }
        exit(0);
    }
}

void MainWorker::processMessage(IrcPrivateMessage* message) {
    if (message->isOwn()) {
        // Ignore our own messages.
        return;
    }
    if (message->isPrivate()) {
        // We will ignore PMs for now.
        return;
    }

    QVariantMap SourceNetwork;

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() == message->connection()) {
            SourceNetwork = RelayedNetwork;
        }
    }

    if (SourceNetwork.count() == 0) {
        qDebug() << "Can't find source network! Message Ignored" << message;
        return;
    }

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() != message->connection()) {
            IrcConnection* CurrConn = (IrcConnection*) RelayedNetwork["Connection"].value<QObject*>();

            if (message->isAction()) {
                CurrConn->sendRaw(QString("PRIVMSG %1 :%2").arg(RelayedNetwork["Channel"].toString(),
                                  FormatAction(SourceNetwork["ActionFormat"].toString(), message->nick(), message->content())));
            } else {
                CurrConn->sendRaw(QString("PRIVMSG %1 :%2").arg(RelayedNetwork["Channel"].toString(),
                                  FormatMessage(SourceNetwork["MessageFormat"].toString(), message->nick(), message->content())));
            }
        }
    }
}

void MainWorker::processNickChange(IrcNickMessage* message) {
    if (message->isOwn()) {
        // Ignore our own nick changes.
        return;
    }

    QVariantMap SourceNetwork;

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() == message->connection()) {
            SourceNetwork = RelayedNetwork;
        }
    }

    if (SourceNetwork.count() == 0) {
        qDebug() << "Can't find source network! Message Ignored" << message;
        return;
    }

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() != message->connection()) {
            IrcConnection* CurrConn = (IrcConnection*) RelayedNetwork["Connection"].value<QObject*>();

            CurrConn->sendRaw(QString("PRIVMSG %1 :%2").arg(RelayedNetwork["Channel"].toString(),
                              FormatNickChangeMessage(SourceNetwork["NickChangeFormat"].toString(), message->oldNick(), message->newNick())));
        }
    }
}

void MainWorker::processJoin(IrcJoinMessage* message) {
    if (message->isOwn()) {
        // If it is us joining, we should set the topic if needed.
        if (GlobalTopic != "") {
            // Leave a delay in case services still didnt give us op.
            IrcConnection* CurrConn = message->connection();
            QString Channel = message->channel();
            QTimer::singleShot(1500, this,
                        [CurrConn, Channel, this]() {
                            CurrConn->sendRaw(QString("TOPIC %1 :%2").arg(Channel, this->GlobalTopic));
                        });
        }
        return;
    }

    QVariantMap SourceNetwork;

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() == message->connection()) {
            SourceNetwork = RelayedNetwork;
        }
    }

    if (SourceNetwork.count() == 0) {
        qDebug() << "Can't find source network! Message Ignored" << message;
        return;
    }

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() != message->connection()) {
            IrcConnection* CurrConn = (IrcConnection*) RelayedNetwork["Connection"].value<QObject*>();

            CurrConn->sendRaw(QString("PRIVMSG %1 :%2").arg(RelayedNetwork["Channel"].toString(),
                              FormatJoinMessage(SourceNetwork["JoinFormat"].toString(), message->nick(), message->ident(), message->host())));
        }
    }
}

void MainWorker::processPart(IrcPartMessage* message) {
    if (message->isOwn()) {
        // Ignore our own join.
        return;
    }

    QVariantMap SourceNetwork;

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() == message->connection()) {
            SourceNetwork = RelayedNetwork;
        }
    }

    if (SourceNetwork.count() == 0) {
        qDebug() << "Can't find source network! Message Ignored" << message;
        return;
    }

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() != message->connection()) {
            IrcConnection* CurrConn = (IrcConnection*) RelayedNetwork["Connection"].value<QObject*>();

            CurrConn->sendRaw(QString("PRIVMSG %1 :%2").arg(RelayedNetwork["Channel"].toString(),
                              FormatPartMessage(SourceNetwork["PartFormat"].toString(), message->nick(), message->ident(), message->host())));
        }
    }
}

void MainWorker::processQuit(IrcQuitMessage* message) {
    if (message->isOwn()) {
        // Ignore our own join.
        return;
    }

    QVariantMap SourceNetwork;

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() == message->connection()) {
            SourceNetwork = RelayedNetwork;
        }
    }

    if (SourceNetwork.count() == 0) {
        qDebug() << "Can't find source network! Message Ignored" << message;
        return;
    }

    foreach (QVariantMap RelayedNetwork, RelayNetworks) {
        if (RelayedNetwork["Connection"].value<QObject*>() != message->connection()) {
            IrcConnection* CurrConn = (IrcConnection*) RelayedNetwork["Connection"].value<QObject*>();

            CurrConn->sendRaw(QString("PRIVMSG %1 :%2").arg(RelayedNetwork["Channel"].toString(),
                              FormatQuitMessage(SourceNetwork["QuitFormat"].toString(), message->nick(), message->ident(), message->host(), message->reason())));
        }
    }
}

QString MainWorker::FormatTopic(QString Format) {
    QString ReturnVal = Format;

    ReturnVal = ReturnVal.replace("${Color}", ColorCode);
    ReturnVal = ReturnVal.replace("${Bold}", BoldCode);
    ReturnVal = ReturnVal.replace("${Italic}", ItalicCode);
    ReturnVal = ReturnVal.replace("${Underline}", UnderlineCode);

    //qDebug() << "Formatted message:" << ReturnVal;

    return ReturnVal;
}
QString MainWorker::FormatMessage(QString Format, QString Nick, QString Message) {
    QString ReturnVal = Format;

    ReturnVal = ReturnVal.replace("${Color}", ColorCode);
    ReturnVal = ReturnVal.replace("${Bold}", BoldCode);
    ReturnVal = ReturnVal.replace("${Italic}", ItalicCode);
    ReturnVal = ReturnVal.replace("${Underline}", UnderlineCode);
    ReturnVal = ReturnVal.replace("${SentBy}", Nick);
    ReturnVal = ReturnVal.replace("${Message}", Message);

    //qDebug() << "Formatted message:" << ReturnVal;

    return ReturnVal;
}
QString MainWorker::FormatAction(QString Format, QString Nick, QString Action) {
    QString ReturnVal = Format;

    ReturnVal = ReturnVal.replace("${Color}", ColorCode);
    ReturnVal = ReturnVal.replace("${Bold}", BoldCode);
    ReturnVal = ReturnVal.replace("${Italic}", ItalicCode);
    ReturnVal = ReturnVal.replace("${Underline}", UnderlineCode);
    ReturnVal = ReturnVal.replace("${SentBy}", Nick);
    ReturnVal = ReturnVal.replace("${Action}", Action);

    //qDebug() << "Formatted message:" << ReturnVal;

    return ReturnVal;
}
QString MainWorker::FormatNickChangeMessage(QString Format, QString OldNick, QString NewNick) {
    QString ReturnVal = Format;

    ReturnVal = ReturnVal.replace("${Color}", ColorCode);
    ReturnVal = ReturnVal.replace("${Bold}", BoldCode);
    ReturnVal = ReturnVal.replace("${Italic}", ItalicCode);
    ReturnVal = ReturnVal.replace("${Underline}", UnderlineCode);
    ReturnVal = ReturnVal.replace("${OldNick}", OldNick);
    ReturnVal = ReturnVal.replace("${NewNick}", NewNick);

    //qDebug() << "Formatted message:" << ReturnVal;

    return ReturnVal;
}
QString MainWorker::FormatJoinMessage(QString Format, QString Nick, QString Ident, QString Host) {
    QString ReturnVal = Format;

    ReturnVal = ReturnVal.replace("${Color}", ColorCode);
    ReturnVal = ReturnVal.replace("${Bold}", BoldCode);
    ReturnVal = ReturnVal.replace("${Italic}", ItalicCode);
    ReturnVal = ReturnVal.replace("${Underline}", UnderlineCode);
    ReturnVal = ReturnVal.replace("${Nick}", Nick);
    ReturnVal = ReturnVal.replace("${Host}", Ident + "@" + Host);

    //qDebug() << "Formatted message:" << ReturnVal;

    return ReturnVal;
}
QString MainWorker::FormatPartMessage(QString Format, QString Nick, QString Ident, QString Host) {
    QString ReturnVal = Format;

    ReturnVal = ReturnVal.replace("${Color}", ColorCode);
    ReturnVal = ReturnVal.replace("${Bold}", BoldCode);
    ReturnVal = ReturnVal.replace("${Italic}", ItalicCode);
    ReturnVal = ReturnVal.replace("${Underline}", UnderlineCode);
    ReturnVal = ReturnVal.replace("${Nick}", Nick);
    ReturnVal = ReturnVal.replace("${Host}", Ident + "@" + Host);

    //qDebug() << "Formatted message:" << ReturnVal;

    return ReturnVal;
}
QString MainWorker::FormatQuitMessage(QString Format, QString Nick, QString Ident, QString Host, QString QuitMsg) {
    QString ReturnVal = Format;

    ReturnVal = ReturnVal.replace("${Color}", ColorCode);
    ReturnVal = ReturnVal.replace("${Bold}", BoldCode);
    ReturnVal = ReturnVal.replace("${Italic}", ItalicCode);
    ReturnVal = ReturnVal.replace("${Underline}", UnderlineCode);
    ReturnVal = ReturnVal.replace("${Nick}", Nick);
    ReturnVal = ReturnVal.replace("${Host}", Ident + "@" + Host);
    ReturnVal = ReturnVal.replace("${QuitMsg}", QuitMsg);

    //qDebug() << "Formatted message:" << ReturnVal;

    return ReturnVal;
}

void MainWorker::ReloadConfig()
{
    QSet<QVariantMap> OldNetworks = RelayNetworks;
    QString OldTopic = GlobalTopic;

    QSet<QVariantMap> ConfigNetworks = ParseConfig();

    foreach (QVariantMap OldNetwork, OldNetworks) {
        if (!containsNetwork(ConfigNetworks, OldNetwork)) {
            IrcConnection* CurrConn = (IrcConnection*) OldNetwork["Connection"].value<QObject*>();
            CurrConn->quit("This network was removed from the config. Bye!");
            delete CurrConn;
            RelayNetworks.remove(OldNetwork);
            qDebug() << "Network removed. New list:" << RelayNetworks;
        }
    }

    OldNetworks = RelayNetworks;

    foreach (QVariantMap ConfigNetwork, ConfigNetworks) {
        IrcConnection* CurrConn = new IrcConnection(this);

        if (!containsNetwork(OldNetworks, ConfigNetwork)) {
            CurrConn->setHost(ConfigNetwork["Hostname"].toString());
            CurrConn->setPort(ConfigNetwork["Port"].toInt());
            CurrConn->setSecure(ConfigNetwork["UseSSL"].toBool());
            if (!ConfigNetwork["ServerPassword"].isNull() && ConfigNetwork["ServerPassword"].toString() != "")
                CurrConn->setPassword(ConfigNetwork["ServerPassword"].toString());
            CurrConn->setNickNames(QStringList({ConfigNetwork["BotNick"].toString(), ConfigNetwork["BotAltNick"].toString()}));
            CurrConn->setUserName(ConfigNetwork["BotUsername"].toString());
            CurrConn->setRealName(ConfigNetwork["BotRealName"].toString());

            if (!ConfigNetwork["CommandOnConnect"].isNull() && ConfigNetwork["CommandOnConnect"].toString() != "")
                CurrConn->sendRaw(ConfigNetwork["CommandOnConnect"].toString());
            CurrConn->sendCommand(IrcCommand::createJoin(ConfigNetwork["Channel"].toString()));

            CurrConn->open();

            connect(CurrConn, &IrcConnection::privateMessageReceived, this, &MainWorker::processMessage);
            connect(CurrConn, &IrcConnection::nickMessageReceived, this, &MainWorker::processNickChange);
            connect(CurrConn, &IrcConnection::joinMessageReceived, this, &MainWorker::processJoin);
            connect(CurrConn, &IrcConnection::partMessageReceived, this, &MainWorker::processPart);
            connect(CurrConn, &IrcConnection::quitMessageReceived, this, &MainWorker::processQuit);

            ConfigNetwork.insert("Connection", QVariant::fromValue<QObject*>(CurrConn));

            RelayNetworks.insert(ConfigNetwork);
        } else {
            // If this network already existed before, check if we need to change the topic there.
            if (OldTopic != GlobalTopic) {
                CurrConn->sendRaw(QString("TOPIC %1 :%2").arg(ConfigNetwork["Channel"].toString(), GlobalTopic));
            }
        }
    }
}

bool MainWorker::containsNetwork(QSet<QVariantMap> A, QVariantMap B) {
    B.remove("Connection");
    foreach (QVariantMap Aelm, A) {
        Aelm.remove("Connection");
        if (Aelm == B)
            return true;
    }
    return false;
}

QSet<QVariantMap> MainWorker::ParseConfig()
{
    QSet<QVariantMap> ReturnVal;

    QFile configFile(configPath);

    if (!configFile.open(QFile::ReadOnly)) {
        qDebug() << "Error opening config file:" << configFile.errorString();
        exit(1);
    }


    QRegExp commentRegex("^[ \\t]*\\/\\/");
    QString ConfigTxt;

    while (!configFile.atEnd()) {
        QString LineRead = configFile.readLine();

        if (commentRegex.indexIn(LineRead) == -1) {
            ConfigTxt += LineRead;
        }
    }

    QJsonParseError configParseError;

    QJsonDocument configDoc = QJsonDocument::fromJson(ConfigTxt.toUtf8(), &configParseError);

    if (configParseError.error != QJsonParseError::NoError) {
        qDebug() << "Invalid config file:" << configParseError.errorString();
        exit(1);
    }

    if (!configDoc.isObject()) {
        qDebug() << "The config must be a JSON object!";
        exit(1);
    }

    QJsonObject configObj = configDoc.object();

    GlobalTopic = configObj["GlobalTopic"].isString() ? FormatTopic(configObj["GlobalTopic"].toString()) : "";

    QJsonArray networksArray = configObj["Networks"].toArray();

    foreach (const QJsonValue & value, networksArray) {
        QJsonObject networkObj = value.toObject();

        ReturnVal.insert(networkObj.toVariantMap());
    }

    return ReturnVal;
}

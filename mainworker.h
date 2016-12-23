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

    QSet<QVariantMap> RelayNetworks;

    void ReloadConfig();

private:

    void processMessage(IrcPrivateMessage* message);
    void processNickChange(IrcNickMessage* message);
    void processJoin(IrcJoinMessage* message);
    void processPart(IrcPartMessage* message);
    void processQuit(IrcQuitMessage* message);

    const QString ColorCode = "\x03";

    QSet<QVariantMap> ParseConfig();

    bool containsNetwork(QSet<QVariantMap> A, QVariantMap B);

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

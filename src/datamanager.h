#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include "deck.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject* parent = nullptr);
    ~DataManager();

    bool load(const QString& filePath = "data.json");
    bool save(const QString& filePath = "data.json") const;

    QList<Deck*>& getDecks();
    const QList<Deck*>& getDecksConst() const;

    void addDeck(Deck* deck);
    bool removeDeck(int index);
    void clearDecks();
    int totalCardCount() const;

    // ★★★ 重点检查这里：必须是 static DataManager* instance(); ★★★
    // 注意：instance 是小写开头，而且后面有个 ()
    static DataManager* instance();

private:
    QList<Deck*> m_decks;
    QString m_currentFilePath;

    // ★★★ 重点检查这里：必须是 static DataManager* ★★★
    static DataManager* m_instance;
};

#endif // DATAMANAGER_H
#include "DataManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// ★★★ 初始化静态变量 ★★★
DataManager* DataManager::m_instance = nullptr;

// ★★★ 重点：这里必须和 .h 里的声明一字不差 ★★★
DataManager* DataManager::instance()
{
    if (!m_instance) {
        m_instance = new DataManager();
    }
    return m_instance;
}

DataManager::DataManager(QObject* parent)
    : QObject(parent)
{
}

DataManager::~DataManager()
{
    clearDecks();
}

bool DataManager::load(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON format or empty file";
        return false;
    }

    QJsonObject root = doc.object();
    if (!root.contains("decks")) {
        qWarning() << "JSON missing 'decks' key";
        return false;
    }

    QJsonArray decksArray = root["decks"].toArray();
    clearDecks();

    for (const QJsonValue& val : decksArray) {
        if (!val.isObject()) {
            qWarning() << "Skipping invalid deck entry";
            continue;
        }

        QJsonObject deckObj = val.toObject();
        Deck* deck = new Deck();
        deck->fromJson(deckObj);
        m_decks.append(deck);
    }

    m_currentFilePath = filePath;
    qDebug() << "Loaded" << m_decks.size() << "decks from" << filePath;
    return true;
}

bool DataManager::save(const QString& filePath) const
{
    QJsonArray decksArray;
    for (const Deck* deck : m_decks) {
        decksArray.append(deck->toJson());
    }

    QJsonObject root;
    root["decks"] = decksArray;

    QJsonDocument doc(root);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    QString savePath = filePath.isEmpty() ? m_currentFilePath : filePath;
    if (savePath.isEmpty()) {
        savePath = "data.json";
    }

    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << savePath;
        return false;
    }

    file.write(data);
    file.close();

    qDebug() << "Saved" << m_decks.size() << "decks to" << savePath;
    return true;
}

QList<Deck*>& DataManager::getDecks()
{
    return m_decks;
}

const QList<Deck*>& DataManager::getDecksConst() const
{
    return m_decks;
}

void DataManager::addDeck(Deck* deck)
{
    if (deck && !m_decks.contains(deck)) {
        m_decks.append(deck);
    }
}

bool DataManager::removeDeck(int index)
{
    if (index < 0 || index >= m_decks.size()) {
        return false;
    }

    delete m_decks[index];
    m_decks.removeAt(index);
    return true;
}

void DataManager::clearDecks()
{
    for (Deck* deck : m_decks) {
        delete deck;
    }
    m_decks.clear();
}

int DataManager::totalCardCount() const
{
    int count = 0;
    for (const Deck* deck : m_decks) {
        count += deck->getCards().size();
    }
    return count;
}
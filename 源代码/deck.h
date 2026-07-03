#ifndef DECK_H
#define DECK_H

#include "card.h"
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QJsonArray>
#include <QJsonValue>

class Deck {
public:
    Deck();
    Deck(const QString& name);

    // 基本属性
    QString getName() const;
    void setName(const QString& name);

    // 卡片管理
    void addCard(const Card& card);
    void removeCard(int index);
    Card& getCard(int index);
    const Card& getCard(int index) const;
    int cardCount() const;
    QList<Card>& getCards();
    const QList<Card>& getCards() const;

    // 到期卡片
    QList<Card*> getDueCards();

    // JSON序列化（提供给C同学的接口）
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

public:
    QList<Card> cards;  // 公开以便DataManager直接访问（根据C同学的代码）

private:
    QString name;
};

#endif // DECK_H
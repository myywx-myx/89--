#include "deck.h"
#include <QJsonArray>
#include <QJsonValue>

Deck::Deck() : name("未命名牌组") {}

Deck::Deck(const QString& name) : name(name) {}

QString Deck::getName() const { return name; }
void Deck::setName(const QString& n) { name = n; }

void Deck::addCard(const Card& card) {
    cards.append(card);
}

void Deck::removeCard(int index) {
    if (index >= 0 && index < cards.size()) {
        cards.removeAt(index);
    }
}

Card& Deck::getCard(int index) {
    return cards[index];
}

const Card& Deck::getCard(int index) const {
    return cards[index];
}


int Deck::cardCount() const {
    return cards.size();
}

QList<Card>& Deck::getCards() {
    return cards;
}

const QList<Card>& Deck::getCards() const {
    return cards;
}

QList<Card*> Deck::getDueCards() {
    QList<Card*> dueCards;
    for (Card& card : cards) {
        if (card.isDue()) {
            dueCards.append(&card);
        }
    }
    return dueCards;
}

QJsonObject Deck::toJson() const {
    QJsonObject json;
    json["name"] = name;

    QJsonArray cardsArray;
    for (const Card& card : cards) {
        cardsArray.append(card.toJsonObject());
    }
    json["cards"] = cardsArray;

    return json;
}

void Deck::fromJson(const QJsonObject& json) {
    name = json["name"].toString();
    cards.clear();

    QJsonArray cardsArray = json["cards"].toArray();
    for (const QJsonValue& val : cardsArray) {
        Card card;
        card.fromJsonObject(val.toObject());
        cards.append(card);
    }
}
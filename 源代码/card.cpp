#include "card.h"
#include <QDebug>
#include <QJsonValue>
#include <QJsonObject>

Card::Card()
    : interval(1), easeFactor(2.5),
    totalReviews(0), sumRatings(0) {
    nextReviewDate = QDate::currentDate();
}

Card::Card(const QString& question, const QString& answer)
    : question(question), answer(answer), interval(1), easeFactor(2.5),totalReviews(0), sumRatings(0) {
    nextReviewDate = QDate::currentDate();
}

QString Card::getQuestion() const { return question; }
QString Card::getAnswer() const { return answer; }
QDate Card::getNextReviewDate() const { return nextReviewDate; }
int Card::getInterval() const { return interval; }
double Card::getEaseFactor() const { return easeFactor; }

void Card::setQuestion(const QString& q) { question = q; }
void Card::setAnswer(const QString& a) { answer = a; }
void Card::setNextReviewDate(const QDate& date) { nextReviewDate = date; }
void Card::setInterval(int i) { interval = i; }
void Card::setEaseFactor(double ef) { easeFactor = ef; }
void Card::setTotalReviews(int t) { totalReviews = t; }
void Card::setSumRatings(int s) { sumRatings = s; }
QJsonObject Card::toJsonObject() const {
    QJsonObject json;
    json["question"] = question;
    json["answer"] = answer;
    json["nextReviewDate"] = nextReviewDate.toString("yyyy-MM-dd");
    json["interval"] = interval;
    json["easeFactor"] = easeFactor;
    json["totalReviews"] = totalReviews;
    json["sumRatings"] = sumRatings;
    return json;
}

void Card::fromJsonObject(const QJsonObject& json) {
    question = json["question"].toString();
    answer = json["answer"].toString();
    nextReviewDate = QDate::fromString(json["nextReviewDate"].toString(), "yyyy-MM-dd");
    interval = json["interval"].toInt();
    easeFactor = json["easeFactor"].toDouble();
    totalReviews = json["totalReviews"].toInt(0);
    sumRatings = json["sumRatings"].toInt(0);
}

bool Card::isDue() const {
    return QDate::currentDate() >= nextReviewDate;
}

int Card::getTotalReviews() const {
    return totalReviews;
}

int Card::getSumRatings() const {
    return sumRatings;
}

double Card::getFamiliarity() const {
    if (totalReviews == 0) return 0.0; // 从未复习过，熟悉度为0（最陌生）
    return static_cast<double>(sumRatings) / totalReviews;
}

void Card::resetReviewData() {
    interval = 1;
    easeFactor = 2.5;
    nextReviewDate = QDate::currentDate();
    totalReviews = 0;
    sumRatings = 0;
}
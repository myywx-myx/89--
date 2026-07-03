#ifndef CARD_H
#define CARD_H

#include <QDate>
#include <QJsonObject>
#include <QString>
#include <QJsonValue>

class Card {
public:
    Card();
    Card(const QString& question, const QString& answer);


    // 获取和设置属性
    QString getQuestion() const;
    QString getAnswer() const;
    QDate getNextReviewDate() const;
    int getInterval() const;
    double getEaseFactor() const;

    void setQuestion(const QString& question);
    void setAnswer(const QString& answer);
    void setNextReviewDate(const QDate& date);
    void setInterval(int interval);
    void setEaseFactor(double easeFactor);
    void setTotalReviews(int t);
    void setSumRatings(int s);

    // JSON序列化（提供给C同学的接口）
    QJsonObject toJsonObject() const;
    void fromJsonObject(const QJsonObject& json);

    // 检查是否到期
    bool isDue() const;

    int getTotalReviews() const;
    int getSumRatings() const;
    double getFamiliarity() const; // 熟悉度：sumRatings/totalReviews，越熟悉值越高
    void resetReviewData();

private:
    QString question;
    QString answer;
    QDate nextReviewDate;  // 下次复习日期
    int interval;          // 当前间隔（天）
    double easeFactor;
    int totalReviews;      // 总复习次数
    int sumRatings;      // 易度因子
};

#endif // CARD_H
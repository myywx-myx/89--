#include "ReviewScheduler.h"
#include <QDate>
#include <QtMath>
#include "card.h"
#include "deck.h"
#include <QDebug>

void ReviewScheduler::updateCard(Card& card, int rating) {
    QDate today = QDate::currentDate();
    int interval = card.getInterval();
    double easeFactor = card.getEaseFactor();

    if (rating == 0) { // 生疏
        interval = 1;
        card.setNextReviewDate(today.addDays(1));
        card.setEaseFactor(2.5);
    }
    else if (rating == 1) { // 犹豫
        interval = qMax(1, static_cast<int>(interval * 1.2));
        card.setNextReviewDate(today.addDays(interval));
        card.setEaseFactor(qMax(1.3, easeFactor - 0.15));
    }
    else if (rating == 2) { // 熟练
        if (interval == 0) interval = 1;
        else interval = static_cast<int>(interval * easeFactor);

        card.setNextReviewDate(today.addDays(interval));
        card.setEaseFactor(qMin(3.0, easeFactor + 0.1));
    }

    card.setInterval(interval);

    // 保险起见，如果下次复习日期在今天或之前，设为明天
    if (card.getNextReviewDate() <= today) {
        card.setNextReviewDate(today.addDays(1));
    }

    card.setTotalReviews(card.getTotalReviews() + 1);
    card.setSumRatings(card.getSumRatings() + rating);
}

QList<Card*> ReviewScheduler::getDueCards(Deck& deck) {
    return deck.getDueCards();
}
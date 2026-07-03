#ifndef REVIEWSCHEDULER_H
#define REVIEWSCHEDULER_H

#include "card.h"
#include "deck.h"
#include <QList>

class ReviewScheduler {
public:
    // 更新卡片状态（根据评分0/1/2）
    void updateCard(Card& card, int rating);

    // 获取到期卡片
    QList<Card*> getDueCards(Deck& deck);
};

#endif // REVIEWSCHEDULER_H
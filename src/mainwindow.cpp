#include "mainwindow.h"
#include <QInputDialog>
#include <QRandomGenerator>

static void fixButton(QPushButton* btn) {
    btn->setMinimumSize(80, 32);
    btn->setStyleSheet(R"(
        QPushButton {
            padding: 6px 12px;
            border-radius: 4px;
            font-size: 14px;
            border: none;
        }
        QPushButton:enabled {
            color: white;
            background-color: #409eff;
        }
        QPushButton:disabled {
            color: #606266;
            background-color: #f0f0f0;
        }
    )");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentDeckIndex(-1)
    , currentCardIndex(-1)
    , currentMemoryCardIndex(-1)
    , currentReviewCardIndex(-1)
    , currentRatingCardIndex(-1)
    , reviewCompleted(false)
{
    dataManager = new DataManager(this);
    dataManager->load();
    setupUI();
    resize(900, 600);
    setWindowTitle("复习卡片应用");

    refreshDeckList();
}

MainWindow::~MainWindow() {
    delete dataManager;
}

void MainWindow::setupUI() {
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    setupStartPage();
    setupFuncSelectPage();
    setupManagePage();
    setupMemoryPage();
    setupReviewPage();
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::setupStartPage() {
    startPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(startPage);
    layout->setAlignment(Qt::AlignCenter);

    // ★★★ 修改为主应用名称，并美化样式 ★★★
    QLabel* titleLabel = new QLabel("📚 记忆卡片");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #2c3e50; padding: 40px;");

    QLabel* subtitleLabel = new QLabel("高效复习，轻松记忆");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 18px; color: #7f8c8d; padding-bottom: 30px;");

    // 保留统计信息（可选，如果你想展示卡片总数可以保留，否则可以去掉）
    statsLabel = new QLabel("统计信息加载中...");
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setStyleSheet("font-size: 16px; color: #95a5a6; padding: 10px;");
    statsLabel->hide();  // 默认隐藏，如果需要显示可改为 show()

    changeBgBtn = new QPushButton("更换背景");
    clearBgBtn = new QPushButton("清除背景");
    enterAppBtn = new QPushButton("进入应用");
    fixButton(changeBgBtn);
    fixButton(clearBgBtn);
    fixButton(enterAppBtn);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(changeBgBtn);
    btnLayout->addWidget(clearBgBtn);
    btnLayout->addWidget(enterAppBtn);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    // layout->addWidget(statsLabel);  // 如果想显示统计信息，取消注释
    layout->addLayout(btnLayout);
    layout->addStretch();

    connect(enterAppBtn, &QPushButton::clicked, this, &MainWindow::onEnterApp);
    connect(changeBgBtn, &QPushButton::clicked, this, &MainWindow::onChangeBg);
    connect(clearBgBtn, &QPushButton::clicked, this, &MainWindow::onClearBg);

    stackedWidget->addWidget(startPage);
}

void MainWindow::setupFuncSelectPage() {
    funcSelectPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(funcSelectPage);
    layout->setAlignment(Qt::AlignCenter);

    manageBtn = new QPushButton("🗂️ 卡片管理\n增删改查卡组与卡片");
    memoryBtn = new QPushButton("🧠 记忆复习\n无限制快速浏览知识点");
    reviewBtn = new QPushButton("📝 自测\n无限制自由复习");
    fixButton(manageBtn);
    fixButton(memoryBtn);
    fixButton(reviewBtn);

    // ★★★ 添加返回主页按钮 ★★★
    QPushButton* homeBtn = new QPushButton("🏠 返回主页");
    fixButton(homeBtn);
    connect(homeBtn, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(0);
    });

    layout->addStretch();
    layout->addWidget(manageBtn);
    layout->addWidget(memoryBtn);
    layout->addWidget(reviewBtn);
    layout->addSpacing(20);           // 与功能按钮之间留空
    layout->addWidget(homeBtn);
    layout->addStretch();

    connect(manageBtn, &QPushButton::clicked, [this](){ stackedWidget->setCurrentIndex(2); });
    connect(memoryBtn, &QPushButton::clicked, [this](){ stackedWidget->setCurrentIndex(3); });
    connect(reviewBtn, &QPushButton::clicked, [this](){ stackedWidget->setCurrentIndex(4); });

    stackedWidget->addWidget(funcSelectPage);
}

void MainWindow::setupManagePage() {
    managePage = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(managePage);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    deckListWidget = new QListWidget();
    addDeckBtn = new QPushButton("添加牌组");
    removeDeckBtn = new QPushButton("删除牌组");
    fixButton(addDeckBtn);
    fixButton(removeDeckBtn);
    connect(deckListWidget, &QListWidget::currentRowChanged, this, &MainWindow::onDeckSelected);
    connect(addDeckBtn, &QPushButton::clicked, this, &MainWindow::onAddDeck);
    connect(removeDeckBtn, &QPushButton::clicked, this, &MainWindow::onRemoveDeck);
    leftLayout->addWidget(new QLabel("牌组列表"));
    leftLayout->addWidget(deckListWidget);
    leftLayout->addWidget(addDeckBtn);
    leftLayout->addWidget(removeDeckBtn);
    renameDeckBtn = new QPushButton("重命名牌组");
    fixButton(renameDeckBtn);
    connect(renameDeckBtn, &QPushButton::clicked, this, &MainWindow::onRenameDeck);
    renameDeckBtn->setEnabled(false); // 初始禁用
    leftLayout->addWidget(renameDeckBtn);

    QVBoxLayout* midLayout = new QVBoxLayout();
    cardListWidget = new QListWidget();
    addCardBtn = new QPushButton("添加卡片");
    removeCardBtn = new QPushButton("删除卡片");
    fixButton(addCardBtn);
    fixButton(removeCardBtn);
    connect(cardListWidget, &QListWidget::currentRowChanged, this, &MainWindow::onCardSelected);
    connect(addCardBtn, &QPushButton::clicked, this, &MainWindow::onAddCard);
    connect(removeCardBtn, &QPushButton::clicked, this, &MainWindow::onRemoveCard);
    midLayout->addWidget(new QLabel("卡片列表"));
    midLayout->addWidget(cardListWidget);
    midLayout->addWidget(addCardBtn);
    midLayout->addWidget(removeCardBtn);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    questionEdit = new QTextEdit();
    answerEdit = new QTextEdit();
    updateCardBtn = new QPushButton("更新卡片");
    searchEdit = new QLineEdit();
    fixButton(updateCardBtn);
    searchEdit->setPlaceholderText("搜索问题或答案...");
    connect(updateCardBtn, &QPushButton::clicked, this, &MainWindow::onUpdateCard);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchCard);
    rightLayout->addWidget(new QLabel("问题"));
    rightLayout->addWidget(questionEdit);
    rightLayout->addWidget(new QLabel("答案"));
    rightLayout->addWidget(answerEdit);
    rightLayout->addWidget(updateCardBtn);
    rightLayout->addWidget(searchEdit);

    QPushButton* backFromManageBtn = new QPushButton("← 返回菜单");
    fixButton(backFromManageBtn);
    connect(backFromManageBtn, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(1);
    });
    rightLayout->addWidget(backFromManageBtn);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(midLayout, 2);
    mainLayout->addLayout(rightLayout, 2);

    addCardBtn->setEnabled(false);
    removeCardBtn->setEnabled(false);

    stackedWidget->addWidget(managePage);
}

void MainWindow::setupMemoryPage() {
    memoryReviewPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(memoryReviewPage);

    memoryDeckCombo = new QComboBox();
    memoryDeckCombo->setMinimumSize(200, 32);
    connect(memoryDeckCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onMemoryDeckChanged);

    memoryQuestionLabel = new QLabel("请先选择卡组，再点击「开始记忆复习」");
    memoryQuestionLabel->setWordWrap(true);
    memoryQuestionLabel->setMinimumHeight(150);
    memoryQuestionLabel->setStyleSheet("padding: 24px; background: white; border-radius: 12px;");

    memoryAnswerLabel = new QLabel("答案将在这里显示");
    memoryAnswerLabel->setWordWrap(true);
    memoryAnswerLabel->setMinimumHeight(100);
    memoryAnswerLabel->setStyleSheet("padding: 24px; background: #f0f9ff; border-radius: 12px; margin-top: 10px;");
    memoryAnswerLabel->hide();

    memoryProgress = new QProgressBar();
    memoryProgress->setTextVisible(true);
    memoryProgress->setFixedHeight(8);

    startMemoryBtn = new QPushButton("▶ 开始记忆复习");
    fixButton(startMemoryBtn);
    startMemoryBtn->setEnabled(true);

    memoryNextBtn = new QPushButton("下一张卡片");
    memoryEndBtn = new QPushButton("✅ 结束复习");
    fixButton(memoryNextBtn);
    fixButton(memoryEndBtn);
    // 初始状态：开始按钮可用，下一张和结束按钮禁用
    memoryNextBtn->setEnabled(false);
    memoryEndBtn->setEnabled(false);

    connect(startMemoryBtn, &QPushButton::clicked, this, &MainWindow::onStartMemoryReview);
    connect(memoryNextBtn, &QPushButton::clicked, this, &MainWindow::onMemoryNextCard);
    connect(memoryEndBtn, &QPushButton::clicked, this, &MainWindow::onMemoryReviewEnd);

    // 返回按钮
    QPushButton* backFromMemoryBtn = new QPushButton("← 返回菜单");
    fixButton(backFromMemoryBtn);
    connect(backFromMemoryBtn, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(1);
    });

    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(startMemoryBtn);
    ctrlLayout->addWidget(memoryNextBtn);
    ctrlLayout->addWidget(memoryEndBtn);
    ctrlLayout->addWidget(backFromMemoryBtn);

    layout->addWidget(memoryDeckCombo);
    layout->addWidget(memoryQuestionLabel);
    layout->addWidget(memoryAnswerLabel);
    layout->addWidget(memoryProgress);
    layout->addLayout(ctrlLayout);

    stackedWidget->addWidget(memoryReviewPage);
}

void MainWindow::setupReviewPage() {
    reviewPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(reviewPage);

    reviewDeckCombo = new QComboBox();
    reviewDeckCombo->setMinimumSize(200, 32);
    connect(reviewDeckCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onReviewDeckChanged);

    reviewQuestionLabel = new QLabel("请先选择卡组，再点击「开始自测」");
    reviewQuestionLabel->setWordWrap(true);
    reviewQuestionLabel->setMinimumHeight(120);
    reviewQuestionLabel->setStyleSheet("padding: 32px; background: white; border-radius: 12px;");

    reviewAnswerLabel = new QLabel("答案将在这里显示");
    reviewAnswerLabel->setWordWrap(true);
    reviewAnswerLabel->setMinimumHeight(80);
    reviewAnswerLabel->setStyleSheet("padding: 32px; background: #f0f9ff; border-radius: 12px; margin-top: 10px;");
    reviewAnswerLabel->hide();

    // 评分按钮（初始隐藏）
    QHBoxLayout* rateLayout = new QHBoxLayout();
    rateFamiliarBtn = new QPushButton("✅ 熟练");
    rateHesitantBtn = new QPushButton("🤔 犹豫");
    rateUnfamiliarBtn = new QPushButton("❌ 陌生");
    fixButton(rateFamiliarBtn);
    fixButton(rateHesitantBtn);
    fixButton(rateUnfamiliarBtn);
    rateFamiliarBtn->setStyleSheet("QPushButton { background-color: #67c23a; color: white; padding: 8px 16px; border-radius: 4px; }");
    rateHesitantBtn->setStyleSheet("QPushButton { background-color: #e6a23c; color: white; padding: 8px 16px; border-radius: 4px; }");
    rateUnfamiliarBtn->setStyleSheet("QPushButton { background-color: #f56c6c; color: white; padding: 8px 16px; border-radius: 4px; }");
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);
    connect(rateFamiliarBtn, &QPushButton::clicked, this, &MainWindow::onRateFamiliar);
    connect(rateHesitantBtn, &QPushButton::clicked, this, &MainWindow::onRateHesitant);
    connect(rateUnfamiliarBtn, &QPushButton::clicked, this, &MainWindow::onRateUnfamiliar);
    rateLayout->addWidget(rateFamiliarBtn);
    rateLayout->addWidget(rateHesitantBtn);
    rateLayout->addWidget(rateUnfamiliarBtn);

    reviewProgress = new QProgressBar();
    reviewProgress->setTextVisible(true);
    reviewProgress->setFixedHeight(8);

    startReviewBtn = new QPushButton("▶ 开始自测");
    fixButton(startReviewBtn);
    startReviewBtn->setEnabled(true);

    showAnswerBtn = new QPushButton("显示答案");
    nextBtn = new QPushButton("下一张卡片");
    reviewAgainBtn = new QPushButton("🔄 再来一次");
    reviewEndBtn = new QPushButton("✅ 结束自测");
    clearCacheBtn = new QPushButton("🗑️ 清除缓存");
    fixButton(showAnswerBtn);
    fixButton(nextBtn);
    fixButton(reviewAgainBtn);
    fixButton(reviewEndBtn);
    fixButton(clearCacheBtn);
    showAnswerBtn->setEnabled(false);
    nextBtn->setEnabled(false);
    reviewAgainBtn->setEnabled(false);
    reviewEndBtn->setEnabled(false);
    clearCacheBtn->setEnabled(false); // 完成一轮后才启用

    connect(startReviewBtn, &QPushButton::clicked, this, &MainWindow::onStartReview);
    connect(showAnswerBtn, &QPushButton::clicked, this, &MainWindow::onShowAnswer);
    connect(nextBtn, &QPushButton::clicked, this, &MainWindow::onNextCard);
    connect(reviewAgainBtn, &QPushButton::clicked, this, &MainWindow::onReviewAgain);
    connect(reviewEndBtn, &QPushButton::clicked, this, &MainWindow::onReviewEnd);
    connect(clearCacheBtn, &QPushButton::clicked, this, &MainWindow::onClearCache);

    QPushButton* backFromReviewBtn = new QPushButton("← 返回菜单");
    fixButton(backFromReviewBtn);
    connect(backFromReviewBtn, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(1);
    });

    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(startReviewBtn);
    ctrlLayout->addWidget(showAnswerBtn);
    ctrlLayout->addWidget(nextBtn);
    ctrlLayout->addWidget(reviewAgainBtn);
    ctrlLayout->addWidget(reviewEndBtn);
    ctrlLayout->addWidget(clearCacheBtn);
    ctrlLayout->addWidget(backFromReviewBtn);

    layout->addWidget(reviewDeckCombo);
    layout->addWidget(reviewQuestionLabel);
    layout->addWidget(reviewAnswerLabel);
    layout->addLayout(rateLayout);
    layout->addWidget(reviewProgress);
    layout->addLayout(ctrlLayout);

    stackedWidget->addWidget(reviewPage);
}

// ================= 功能实现 =================

void MainWindow::refreshDeckList() {
    deckListWidget->clear();
    memoryDeckCombo->clear();
    reviewDeckCombo->clear();
    const QList<Deck*>& decks = dataManager->getDecks();
    int totalCards = 0;
    for (Deck* deck : decks) {
        int cardCount = deck->getCards().size();
        deckListWidget->addItem(QString("%1（共%2张）").arg(deck->getName()).arg(cardCount));
        memoryDeckCombo->addItem(deck->getName());
        reviewDeckCombo->addItem(deck->getName());
        totalCards += cardCount;
    }
    statsLabel->setText(QString("<b>📚 共有 %1 个牌组</b><br><b>📝 共有 %2 张卡片</b>").arg(decks.size()).arg(totalCards));
}

void MainWindow::onEnterApp() {
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::onDeckSelected(int index) {
    currentDeckIndex = index;
    currentCardIndex = -1;
    questionEdit->clear();
    answerEdit->clear();
    refreshCardList();

    if (index >= 0) {
        addCardBtn->setEnabled(true);
        removeCardBtn->setEnabled(false);
        renameDeckBtn->setEnabled(true);  // 启用重命名按钮
    } else {
        addCardBtn->setEnabled(false);
        removeCardBtn->setEnabled(false);
        renameDeckBtn->setEnabled(false); // 禁用重命名按钮
    }
}

void MainWindow::onCardSelected(int index) {
    if (currentDeckIndex < 0) return;
    QListWidgetItem* item = cardListWidget->item(index);
    if (!item) {
        currentCardIndex = -1;
        questionEdit->clear();
        answerEdit->clear();
        removeCardBtn->setEnabled(false);
        return;
    }
    // 从自定义数据中获取原始索引
    int originalIndex = item->data(Qt::UserRole).toInt();
    currentCardIndex = originalIndex;
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    if (originalIndex < 0 || originalIndex >= deck->getCards().size()) {
        return;
    }
    Card& card = deck->getCards()[originalIndex];
    questionEdit->setText(card.getQuestion());
    answerEdit->setText(card.getAnswer());
    removeCardBtn->setEnabled(true);
}

void MainWindow::onAddDeck() {
    bool ok;
    QString name = QInputDialog::getText(this, "添加牌组", "请输入牌组名称:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        dataManager->addDeck(new Deck(name));
        dataManager->save();
        refreshDeckList();
    }
}

void MainWindow::onRemoveDeck() {
    if (currentDeckIndex < 0) return;
    if (QMessageBox::question(this, "确认", "删除牌组？") == QMessageBox::Yes) {
        dataManager->removeDeck(currentDeckIndex);
        dataManager->save();
        currentDeckIndex = -1;
        refreshDeckList();
        cardListWidget->clear();
        questionEdit->clear();
        answerEdit->clear();
    }
}

void MainWindow::onAddCard() {
    if (currentDeckIndex < 0) {
        QMessageBox::warning(this, "警告", "请先选择牌组");
        return;
    }
    QString q = questionEdit->toPlainText().trimmed();
    QString a = answerEdit->toPlainText().trimmed();
    if (q.isEmpty() || a.isEmpty()) {
        QMessageBox::warning(this, "警告", "问题和答案不能为空");
        return;
    }
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    deck->addCard(Card(q, a));
    dataManager->save();
    refreshCardList();
    refreshDeckList();
    questionEdit->clear();
    answerEdit->clear();
    currentCardIndex = -1;
}

void MainWindow::onUpdateCard() {
    if (currentDeckIndex < 0 || currentCardIndex < 0) {
        QMessageBox::warning(this, "警告", "请先选择卡片");
        return;
    }
    QString q = questionEdit->toPlainText().trimmed();
    QString a = answerEdit->toPlainText().trimmed();
    if (q.isEmpty() || a.isEmpty()) {
        QMessageBox::warning(this, "警告", "问题和答案不能为空");
        return;
    }
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    Card& card = deck->getCards()[currentCardIndex];
    card.setQuestion(q);
    card.setAnswer(a);
    dataManager->save();
    refreshCardList();
}

void MainWindow::onRemoveCard() {
    if (currentDeckIndex < 0 || currentCardIndex < 0) return;
    if (QMessageBox::question(this, "确认", "删除卡片？") == QMessageBox::Yes) {
        Deck* deck = dataManager->getDecks()[currentDeckIndex];
        deck->removeCard(currentCardIndex);
        dataManager->save();
        refreshCardList();
        refreshDeckList();
        questionEdit->clear();
        answerEdit->clear();
        currentCardIndex = -1;
    }
}

void MainWindow::onSearchCard(const QString& keyword) {
    if (currentDeckIndex < 0) return;
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    cardListWidget->clear();
    int originalIndex = 0;
    for (const Card& card : deck->getCards()) {
        if (keyword.isEmpty() ||
            card.getQuestion().contains(keyword, Qt::CaseInsensitive) ||
            card.getAnswer().contains(keyword, Qt::CaseInsensitive)) {
            QListWidgetItem* item = new QListWidgetItem(card.getQuestion().left(30));
            item->setData(Qt::UserRole, originalIndex); // 存储原始索引
            cardListWidget->addItem(item);
        }
        ++originalIndex;
    }
}

void MainWindow::onMemoryDeckChanged(int index) {
    // 保存当前卡组的进度（如果有）
    if (currentDeckIndex >= 0 && currentMemoryCardIndex >= 0 && !memoryCards.isEmpty()) {
        savedMemoryCards[currentDeckIndex] = memoryCards;
        savedMemoryIndex[currentDeckIndex] = currentMemoryCardIndex;
    }

    currentDeckIndex = index;
    // 加载新卡组的进度（如果有）
    if (savedMemoryCards.contains(index) && savedMemoryIndex.value(index, -1) >= 0) {
        memoryCards = savedMemoryCards.value(index);
        currentMemoryCardIndex = savedMemoryIndex.value(index);
        memoryQuestionLabel->setText("检测到上次未完成的复习，点击「开始记忆复习」可选择继续");
    } else {
        memoryCards.clear();
        currentMemoryCardIndex = -1;
        memoryQuestionLabel->setText("请先选择卡组，再点击「开始记忆复习」");
    }
    memoryAnswerLabel->hide();
    memoryProgress->reset();
    startMemoryBtn->setEnabled(true);
    memoryDeckCombo->setEnabled(true);
    memoryNextBtn->setEnabled(false);
    memoryEndBtn->setEnabled(false);
}

void MainWindow::onStartMemoryReview() {
    if (currentDeckIndex < 0) {
        QMessageBox::warning(this, "警告", "请先选择卡组");
        return;
    }

    // 检查是否有保存的进度
    bool hasProgress = savedMemoryCards.contains(currentDeckIndex) &&
                       savedMemoryIndex.value(currentDeckIndex, -1) >= 0 &&
                       !savedMemoryCards.value(currentDeckIndex).isEmpty();

    if (hasProgress) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "选择模式",
                                                                  "检测到上次未完成的复习，是否继续？\n\n选择「是」继续上次进度\n选择「否」重新开始",
                                                                  QMessageBox::Yes | QMessageBox::No,
                                                                  QMessageBox::Yes);
        if (reply == QMessageBox::Yes) {
            // 继续上次的进度
            memoryCards = savedMemoryCards.value(currentDeckIndex);
            currentMemoryCardIndex = savedMemoryIndex.value(currentDeckIndex);
        } else {
            // 新开始：重新洗牌
            Deck* deck = dataManager->getDecks()[currentDeckIndex];
            memoryCards = deck->getCards();
            if (memoryCards.isEmpty()) {
                QMessageBox::information(this, "提示", "卡组为空");
                return;
            }
            std::shuffle(memoryCards.begin(), memoryCards.end(), *QRandomGenerator::global());
            currentMemoryCardIndex = 0;
            // 清除旧进度
            savedMemoryCards.remove(currentDeckIndex);
            savedMemoryIndex.remove(currentDeckIndex);
        }
    } else {
        // 无进度，正常开始
        Deck* deck = dataManager->getDecks()[currentDeckIndex];
        memoryCards = deck->getCards();
        if (memoryCards.isEmpty()) {
            QMessageBox::information(this, "提示", "卡组为空");
            return;
        }
        std::shuffle(memoryCards.begin(), memoryCards.end(), *QRandomGenerator::global());
        currentMemoryCardIndex = 0;
    }

    // 显示当前卡片
    memoryProgress->setMaximum(memoryCards.size());
    memoryProgress->setValue(currentMemoryCardIndex + 1);
    Card& card = memoryCards[currentMemoryCardIndex];
    memoryQuestionLabel->setText(card.getQuestion());
    memoryAnswerLabel->setText(card.getAnswer());
    memoryAnswerLabel->show();
    startMemoryBtn->setEnabled(false);
    memoryDeckCombo->setEnabled(false);
    memoryNextBtn->setEnabled(true);
    memoryEndBtn->setEnabled(true);
}

void MainWindow::onMemoryNextCard() {
    currentMemoryCardIndex++;
    if (currentMemoryCardIndex >= memoryCards.size()) {
        currentMemoryCardIndex = 0; // 循环
    }
    memoryProgress->setValue(currentMemoryCardIndex + 1);
    Card& card = memoryCards[currentMemoryCardIndex];
    memoryQuestionLabel->setText(card.getQuestion());
    memoryAnswerLabel->setText(card.getAnswer());
    memoryAnswerLabel->show();
    // 按钮状态不变（下一张和结束仍然可用）
}



void MainWindow::onMemoryReviewEnd() {
    // 保存当前进度（不清除，以便下次继续）
    if (currentDeckIndex >= 0 && currentMemoryCardIndex >= 0 && !memoryCards.isEmpty()) {
        savedMemoryCards[currentDeckIndex] = memoryCards;
        savedMemoryIndex[currentDeckIndex] = currentMemoryCardIndex;
    }

    // 重置UI但不清除进度数据
    memoryQuestionLabel->setText("复习已暂停，点击「开始记忆复习」可继续");
    memoryAnswerLabel->hide();
    memoryProgress->reset();
    startMemoryBtn->setEnabled(true);
    memoryDeckCombo->setEnabled(true);
    memoryNextBtn->setEnabled(false);
    memoryEndBtn->setEnabled(false);
    // 注意：不清理 memoryCards 和 currentMemoryCardIndex，保留进度
}

void MainWindow::onReviewDeckChanged(int index) {
    // 保存当前进度（如果有）
    if (currentDeckIndex >= 0 && currentReviewCardIndex >= 0 && !reviewCards.isEmpty()) {
        if (!completedReviewDecks.contains(currentDeckIndex)) {
            savedReviewCards[currentDeckIndex] = reviewCards;
            savedReviewIndex[currentDeckIndex] = currentReviewCardIndex;
        }
    }

    currentDeckIndex = index;
    reviewCompleted = false;
    clearCacheBtn->setEnabled(false);
    // 隐藏评分按钮
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);

    // 加载进度
    bool isCompleted = completedReviewDecks.contains(index);
    if (!isCompleted && savedReviewCards.contains(index) && savedReviewIndex.value(index, -1) >= 0) {
        reviewCards = savedReviewCards.value(index);
        currentReviewCardIndex = savedReviewIndex.value(index);
        reviewQuestionLabel->setText("检测到上次未完成的自测，点击「开始自测」可选择继续");
    } else {
        reviewCards.clear();
        currentReviewCardIndex = -1;
        reviewQuestionLabel->setText("请先选择卡组，再点击「开始自测」");
    }
    reviewAnswerLabel->hide();
    reviewProgress->reset();
    startReviewBtn->setEnabled(true);
    reviewDeckCombo->setEnabled(true);
    showAnswerBtn->setEnabled(false);
    nextBtn->setEnabled(false);
    reviewAgainBtn->setEnabled(false);
    reviewEndBtn->setEnabled(false);
}

void MainWindow::onStartReview() {
    if (currentDeckIndex < 0) {
        QMessageBox::warning(this, "警告", "请先选择卡组");
        return;
    }

    bool isCompleted = completedReviewDecks.contains(currentDeckIndex);
    bool hasProgress = savedReviewCards.contains(currentDeckIndex) &&
                       savedReviewIndex.value(currentDeckIndex, -1) >= 0 &&
                       !savedReviewCards.value(currentDeckIndex).isEmpty();

    if (hasProgress && !isCompleted) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "选择模式",
                                                                  "检测到上次未完成的自测，是否继续？\n\n选择「是」继续上次进度\n选择「否」重新开始",
                                                                  QMessageBox::Yes | QMessageBox::No,
                                                                  QMessageBox::Yes);
        if (reply == QMessageBox::Yes) {
            reviewCards = savedReviewCards.value(currentDeckIndex);
            currentReviewCardIndex = savedReviewIndex.value(currentDeckIndex);
        } else {
            // 重新开始，按熟悉度排序
            loadAndSortCardsByFamiliarity();
            currentReviewCardIndex = 0;
            savedReviewCards.remove(currentDeckIndex);
            savedReviewIndex.remove(currentDeckIndex);
        }
    } else {
        // 无进度或已完成，按熟悉度排序新开始
        loadAndSortCardsByFamiliarity();
        currentReviewCardIndex = 0;
        completedReviewDecks.remove(currentDeckIndex);
        savedReviewCards.remove(currentDeckIndex);
        savedReviewIndex.remove(currentDeckIndex);
    }

    reviewCompleted = false;
    clearCacheBtn->setEnabled(false);
    // 显示第一张卡片
    reviewProgress->setMaximum(reviewCards.size());
    reviewProgress->setValue(currentReviewCardIndex + 1);
    reviewQuestionLabel->setText(reviewCards[currentReviewCardIndex].getQuestion());
    reviewAnswerLabel->hide();
    showAnswerBtn->setEnabled(true);
    nextBtn->setEnabled(false);
    reviewAgainBtn->setEnabled(false);
    reviewEndBtn->setEnabled(true);
    // 隐藏评分按钮
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);

    startReviewBtn->setEnabled(false);
    reviewDeckCombo->setEnabled(false);
}

void MainWindow::loadAndSortCardsByFamiliarity() {
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    reviewCards = deck->getCards();
    if (reviewCards.isEmpty()) {
        QMessageBox::information(this, "提示", "卡组为空");
        return;
    }
    // 按熟悉度升序排序（熟悉度低的优先）
    std::sort(reviewCards.begin(), reviewCards.end(),
              [](const Card& a, const Card& b) {
                  return a.getFamiliarity() < b.getFamiliarity();
              });
}

void MainWindow::onShowAnswer() {
    if (currentReviewCardIndex >= 0 && currentReviewCardIndex < reviewCards.size()) {
        reviewAnswerLabel->setText(reviewCards[currentReviewCardIndex].getAnswer());
    }
    reviewAnswerLabel->show();
    showAnswerBtn->setEnabled(false);
    rateFamiliarBtn->setEnabled(true);
    rateHesitantBtn->setEnabled(true);
    rateUnfamiliarBtn->setEnabled(true);
    nextBtn->setEnabled(false);
}

void MainWindow::onNextCard() {
    currentReviewCardIndex++;
    if (currentReviewCardIndex >= reviewCards.size()) {
        completedReviewDecks.insert(currentDeckIndex);
        savedReviewCards.remove(currentDeckIndex);
        savedReviewIndex.remove(currentDeckIndex);
        reviewCompleted = true;
        clearCacheBtn->setEnabled(true);

        currentReviewCardIndex = reviewCards.size() - 1;
        reviewProgress->setValue(reviewCards.size());
        reviewQuestionLabel->setText(reviewCards.last().getQuestion());
        reviewAnswerLabel->setText(reviewCards.last().getAnswer()); // 显示最后一张答案
        reviewAnswerLabel->show();
        showAnswerBtn->setEnabled(false);
        nextBtn->setEnabled(false);
        reviewAgainBtn->setEnabled(true);
        rateFamiliarBtn->setEnabled(false);
        rateHesitantBtn->setEnabled(false);
        rateUnfamiliarBtn->setEnabled(false);
        return;
    }
    reviewProgress->setValue(currentReviewCardIndex + 1);
    reviewQuestionLabel->setText(reviewCards[currentReviewCardIndex].getQuestion());
    reviewAnswerLabel->hide();          // 隐藏答案
    reviewAnswerLabel->clear();         // 清空文本，避免残留
    showAnswerBtn->setEnabled(true);
    nextBtn->setEnabled(false);
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);
}

void MainWindow::onReviewAgain() {
    completedReviewDecks.remove(currentDeckIndex);
    savedReviewCards.remove(currentDeckIndex);
    savedReviewIndex.remove(currentDeckIndex);
    reviewCompleted = false;
    clearCacheBtn->setEnabled(false);

    loadAndSortCardsByFamiliarity();
    currentReviewCardIndex = 0;
    reviewProgress->setMaximum(reviewCards.size());
    reviewProgress->setValue(1);
    reviewQuestionLabel->setText(reviewCards.first().getQuestion());
    reviewAnswerLabel->hide();          // 隐藏答案
    reviewAnswerLabel->clear();         // 清空文本
    showAnswerBtn->setEnabled(true);
    nextBtn->setEnabled(false);
    reviewAgainBtn->setEnabled(false);
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);
    startReviewBtn->setEnabled(false);
    reviewDeckCombo->setEnabled(false);
}

void MainWindow::onReviewEnd() {
    if (!completedReviewDecks.contains(currentDeckIndex)) {
        // 未完成，保存进度
        if (currentDeckIndex >= 0 && currentReviewCardIndex >= 0 && !reviewCards.isEmpty()) {
            savedReviewCards[currentDeckIndex] = reviewCards;
            savedReviewIndex[currentDeckIndex] = currentReviewCardIndex;
        }
    }

    reviewQuestionLabel->setText("自测已暂停，点击「开始自测」可继续");
    reviewAnswerLabel->hide();
    reviewProgress->reset();
    startReviewBtn->setEnabled(true);
    reviewDeckCombo->setEnabled(true);
    showAnswerBtn->setEnabled(false);
    nextBtn->setEnabled(false);
    reviewAgainBtn->setEnabled(false);
    reviewEndBtn->setEnabled(false);
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);
    // 清除缓存按钮状态由 reviewCompleted 决定
    if (reviewCompleted) {
        clearCacheBtn->setEnabled(true);
    } else {
        clearCacheBtn->setEnabled(false);
    }
}

void MainWindow::onChangeBg() {
    QString path = QFileDialog::getOpenFileName(this, "选择背景", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "图片(*.png *.jpg)");
    if (!path.isEmpty()) {
        QString style = QString("QWidget { background-image: url(%1); background-repeat: no-repeat; background-position: center; background-size: cover; }").arg(path);
        startPage->setStyleSheet(style);
        managePage->setStyleSheet(style);
        memoryReviewPage->setStyleSheet(style);
        reviewPage->setStyleSheet(style);
        QSettings settings("MyCompany", "MemoryCardApp");
        settings.setValue("background/path", path);
        QMessageBox::information(this, "成功", "背景已保存");
    }
}

void MainWindow::onClearBg() {
    startPage->setStyleSheet("");
    managePage->setStyleSheet("");
    memoryReviewPage->setStyleSheet("");
    reviewPage->setStyleSheet("");
    QSettings settings("MyCompany", "MemoryCardApp");
    settings.remove("background/path");
    QMessageBox::information(this, "成功", "背景已重置");
}

void MainWindow::saveData() {
    dataManager->save();
}

void MainWindow::refreshCardList() {
    cardListWidget->clear();
    if (currentDeckIndex < 0) return;
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    int idx = 0;
    for (const Card& card : deck->getCards()) {
        QListWidgetItem* item = new QListWidgetItem(card.getQuestion().left(30));
        item->setData(Qt::UserRole, idx);  // 存储原始索引
        cardListWidget->addItem(item);
        ++idx;
    }
    if (deck->getCards().isEmpty()) {
        removeCardBtn->setEnabled(false);
    }
}
void MainWindow::onRenameDeck() {
    if (currentDeckIndex < 0) {
        QMessageBox::warning(this, "警告", "请先选择牌组");
        return;
    }
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    bool ok;
    QString newName = QInputDialog::getText(this, "重命名牌组", "请输入新名称:", QLineEdit::Normal, deck->getName(), &ok);
    if (ok && !newName.isEmpty()) {
        deck->setName(newName);
        dataManager->save();
        refreshDeckList();  // 刷新所有列表，包括管理页、记忆页、自测页的下拉框
    }
}

void MainWindow::onRateFamiliar() {
    recordRating(2); // 熟练
}

void MainWindow::onRateHesitant() {
    recordRating(1); // 犹豫
}

void MainWindow::onRateUnfamiliar() {
    recordRating(0); // 陌生
}

void MainWindow::recordRating(int rating) {
    if (currentDeckIndex < 0 || currentReviewCardIndex < 0 || currentReviewCardIndex >= reviewCards.size())
        return;

    // 更新当前卡片的评分数据
    Card& card = reviewCards[currentReviewCardIndex];
    card.setTotalReviews(card.getTotalReviews() + 1);
    card.setSumRatings(card.getSumRatings() + rating);

    // 同时更新原始卡组中的卡片数据（保持持久化）
    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    // 注意：reviewCards是副本，需要同步到原卡组
    // 由于reviewCards是从deck->getCards()复制来的，修改副本不影响原卡组。
    // 因此需要找到原卡组中对应的卡片并更新。
    // 简单做法：遍历原卡组，找到相同question和answer的卡片（或根据索引）
    // 更好的做法：在开始自测时记录原始索引映射，但为了简化，我们直接遍历匹配。
    // 由于卡片可能重复，但概率低，我们按内容匹配。
    for (Card& origCard : deck->getCards()) {
        if (origCard.getQuestion() == card.getQuestion() && origCard.getAnswer() == card.getAnswer()) {
            origCard.setTotalReviews(card.getTotalReviews());
            origCard.setSumRatings(card.getSumRatings());
            break;
        }
    }
    dataManager->save(); // 保存到文件

    // 禁用评分按钮，启用下一张
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);
    nextBtn->setEnabled(true);
}

void MainWindow::onClearCache() {
    if (currentDeckIndex < 0) return;
    QMessageBox::StandardButton reply = QMessageBox::question(this, "清除缓存",
                                                              "确定要清除当前卡组所有卡片的评分数据吗？\n这将重置每张卡片的熟悉度。",
                                                              QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    Deck* deck = dataManager->getDecks()[currentDeckIndex];
    for (Card& card : deck->getCards()) {
        card.resetReviewData();
    }
    dataManager->save();
    // 清除完成后，重新加载卡片（按熟悉度排序，此时所有卡片熟悉度为0）
    loadAndSortCardsByFamiliarity();
    // 重置状态
    completedReviewDecks.remove(currentDeckIndex);
    savedReviewCards.remove(currentDeckIndex);
    savedReviewIndex.remove(currentDeckIndex);
    reviewCompleted = false;
    clearCacheBtn->setEnabled(false);
    currentReviewCardIndex = -1;
    reviewQuestionLabel->setText("缓存已清除，点击「开始自测」重新开始");
    reviewAnswerLabel->hide();
    reviewProgress->reset();
    startReviewBtn->setEnabled(true);
    reviewDeckCombo->setEnabled(true);
    showAnswerBtn->setEnabled(false);
    nextBtn->setEnabled(false);
    reviewAgainBtn->setEnabled(false);
    reviewEndBtn->setEnabled(false);
    rateFamiliarBtn->setEnabled(false);
    rateHesitantBtn->setEnabled(false);
    rateUnfamiliarBtn->setEnabled(false);
}
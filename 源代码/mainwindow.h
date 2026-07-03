#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <algorithm>
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QList>
#include <QDebug>
#include <QMap>
#include <QSet>


#include "DataManager.h"

class Card;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshDeckList();
    void refreshCardList();
    void saveData();

    void onEnterApp();
    void onChangeBg();
    void onClearBg();

    void onAddDeck();
    void onRemoveDeck();
    void onRenameDeck();
    void onAddCard();
    void onUpdateCard();
    void onRemoveCard();
    void onSearchCard(const QString& keyword);
    void onDeckSelected(int index);
    void onCardSelected(int index);

    void onMemoryDeckChanged(int index);
    void onStartMemoryReview();          // ★ 添加缺失的声明
    void onMemoryNextCard();
    void onMemoryReviewEnd();

    void onReviewDeckChanged(int index);
    void onStartReview();                // ★ 添加缺失的声明
    void onShowAnswer();
    void onNextCard();
    void onReviewAgain();
    void onReviewEnd();

    void onRateFamiliar();   // 评分：熟练
    void onRateHesitant();   // 评分：犹豫
    void onRateUnfamiliar(); // 评分：陌生
    void onClearCache();     // 清除缓存

    void loadAndSortCardsByFamiliarity(); // 按熟悉度排序加载卡片
    void recordRating(int rating);        // 记录评分

private:
    void setupUI();
    void setupStartPage();
    void setupFuncSelectPage();
    void setupManagePage();
    void setupMemoryPage();
    void setupReviewPage();

    QStackedWidget* stackedWidget;

    QWidget* startPage;
    QLabel* statsLabel;
    QPushButton* enterAppBtn;
    QPushButton* changeBgBtn;
    QPushButton* clearBgBtn;

    QWidget* funcSelectPage;
    QPushButton* manageBtn;
    QPushButton* memoryBtn;
    QPushButton* reviewBtn;

    QWidget* managePage;
    QListWidget* deckListWidget;
    QListWidget* cardListWidget;
    QTextEdit* questionEdit;
    QTextEdit* answerEdit;
    QPushButton* addDeckBtn;
    QPushButton* removeDeckBtn;
    QPushButton* renameDeckBtn;
    QPushButton* addCardBtn;
    QPushButton* updateCardBtn;
    QPushButton* removeCardBtn;
    QLineEdit* searchEdit;

    QWidget* memoryReviewPage;
    QComboBox* memoryDeckCombo;
    QLabel* memoryQuestionLabel;
    QLabel* memoryAnswerLabel;
    QPushButton* memoryNextBtn;
    QProgressBar* memoryProgress;
    QPushButton* memoryAgainBtn;
    QPushButton* memoryEndBtn;
    QPushButton* startMemoryBtn;
    QPushButton* startReviewBtn;

    QWidget* reviewPage;
    QComboBox* reviewDeckCombo;
    QLabel* reviewQuestionLabel;
    QLabel* reviewAnswerLabel;
    QPushButton* showAnswerBtn;
    QPushButton* nextBtn;
    QProgressBar* reviewProgress;
    QPushButton* reviewAgainBtn;
    QPushButton* reviewEndBtn;

    DataManager* dataManager;
    int currentDeckIndex;
    int currentCardIndex;
    int currentMemoryCardIndex;
    int currentReviewCardIndex;

    QList<Card> memoryCards;
    QList<Card> reviewCards;

    QMap<int, QList<Card>> savedMemoryCards; // 按卡组索引保存记忆卡片列表
    QMap<int, int> savedMemoryIndex;         // 保存记忆当前索引

    // 自测进度保存
    QMap<int, QList<Card>> savedReviewCards;
    QMap<int, int> savedReviewIndex;

    QSet<int> completedReviewDecks;

    QPushButton* rateFamiliarBtn;   // 熟练按钮
    QPushButton* rateHesitantBtn;   // 犹豫按钮
    QPushButton* rateUnfamiliarBtn; // 陌生按钮
    QPushButton* clearCacheBtn;     // 清除缓存按钮

    int currentRatingCardIndex;
    bool reviewCompleted;
};

#endif // MAINWINDOW_H
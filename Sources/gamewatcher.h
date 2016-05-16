#ifndef GAMEWATCHER_H
#define GAMEWATCHER_H

#include <QObject>
#include <QString>
#include "Cards/secretcard.h"
#include "utility.h"


class GameResult
{
public:
    bool isFirst, isWinner;
    QString playerHero, enemyHero, enemyName;
};


class ArenaRewards
{
public:
    ArenaRewards(){gold=0; dust=0; packs=0; plainCards=0; goldCards=0;}
    int gold, dust, packs, plainCards, goldCards;
};


class ArenaResult
{
public:
    QString playerHero;
    QList<GameResult> gameResultList;
    ArenaRewards arenaRewards;
};



class GameWatcher : public QObject
{
    Q_OBJECT
public:
    GameWatcher(QObject *parent = 0);
    ~GameWatcher();

private:
    enum PowerState { noGame, heroType1State, heroType2State, mulliganState, inGameState };
    enum ArenaState { noDeckRead, deckRead, readingDeck };

//Variables
private:
    QString playerTag;
    PowerState powerState;
    ArenaState arenaState;
    LoadingScreenState loadingScreenState;
    QString hero1, hero2, name1, name2, firstPlayer, winnerPlayer;
    int playerID;
    SecretHero secretHero;
    int enemyMinions, enemyMinionsAliveForAvenge; //Avenge control
    int playerMinions;
    bool isPlayerTurn;
    QRegularExpressionMatch *match;
    //TurnReal avanza a turn cuando robamos carta, nos aseguramos de que animaciones atrasadas
    //no aparezcan como parte del nuevo turno
    int turn, turnReal;
    bool mulliganEnemyDone, mulliganPlayerDone;
    bool synchronized;
    qint64 logSeekCreate;
    bool copyGameLogs;
    bool spectating;



//Metodos
private:
    void createGameResult(QString logFileName);
    void processLoadingScreen(QString &line, qint64 numLine);
    void processArena(QString &line, qint64 numLine);
    void processPower(QString &line, qint64 numLine, qint64 logSeek);
    void processPowerMulligan(QString &line, qint64 numLine);
    void processPowerInGame(QString &line, qint64 numLine);
    void processZone(QString &line, qint64 numLine);
    void advanceTurn(bool playerDraw);
    void startReadingDeck();
    void endReadingDeck();
    bool isHeroPower(QString code);
    QString createGameLog(qint64 logSeekWon);


public:
    void reset();
    void setSynchronized();
    LoadingScreenState getLoadingScreen();
    void setCopyGameLogs(bool value);

signals:
    void newGameResult(GameResult gameResult, LoadingScreenState loadingScreen, QString logFileName);
    void newArena(QString hero);
    void inRewards();
    void newDeckCard(QString card);
    void startGame();
    void endGame();
    void enemyHero(QString hero);
    void enterArena();
    void leaveArena();
    void playerCardDraw(QString code, bool mulligan=false);
    void playerReturnToDeck(QString code);
    void enemyKnownCardDraw(QString code);
    void enemyCardDraw(int id, int turn=0, bool special=false, QString code="");
    void enemyCardPlayed(int id, QString code="");
    void lastHandCardIsCoin();
    void enemySecretPlayed(int id, SecretHero hero);
    void enemySecretStealed(int id, QString code);
    void enemySecretRevealed(int id, QString code);
    void playerSpellPlayed();
    void playerMinionPlayed(int playerMinions);
    void playerMinionZonePlayAdd(QString code, int id, int pos);
    void enemyMinionZonePlayAdd(QString code, int id, int pos);
    void playerMinionZonePlayRemove(int id);
    void enemyMinionZonePlayRemove(int id);
    void playerMinionZonePlaySteal(int id, int pos);
    void enemyMinionZonePlaySteal(int id, int pos);
    void playerMinionPosChange(int id, int pos);
    void enemyMinionPosChange(int id, int pos);
    void playerMinionTagChange(int id, QString tag, QString value);
    void enemyMinionTagChange(int id, QString tag, QString value);
    void enemyMinionDead(QString code);
    void playerAttack(bool isHeroFrom, bool isHeroTo);
    void playerSpellObjPlayed();
    void playerHeroPower();
    void avengeTested();
    void cSpiritTested();
    void clearDrawList(bool forceClear=false);
    void newTurn(bool isPlayerTurn);
    void needResetDeck();
    void activeDraftDeck();
    void pickCard(QString code);
    void specialCardTrigger(QString code, QString subType);
    void gameLogComplete(qint64 logSeekCreate, qint64 logSeekWon, QString fileName);
    void arenaDeckRead();
    void pLog(QString line);
    void pDebug(QString line, qint64 numLine, DebugLevel debugLevel=Normal, QString file="GameWatcher");

private slots:
    void checkAvenge();

public slots:
    void processLogLine(LogComponent logComponent, QString line, qint64 numLine, qint64 logSeek);
    void setDeckRead(bool value=true);
};

#endif // GAMEWATCHER_H

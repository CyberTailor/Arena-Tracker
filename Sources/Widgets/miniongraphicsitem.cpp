#include "miniongraphicsitem.h"
#include "../utility.h"
#include <QtWidgets>

MinionGraphicsItem::MinionGraphicsItem(QString code, int id, bool friendly, bool playerTurn)
{
    this->code = code;
    this->id = id;
    this->friendly = friendly;
    this->attack = this->origAttack = Utility::getCardAtribute(code, "attack").toInt();
    this->health = this->origHealth = Utility::getCardAtribute(code, "health").toInt();
    this->damage = 0;
    this->shield = false;
    this->taunt = false;
    this->exausted = !friendly;
    this->playerTurn = friendly && playerTurn;//Para minion enemigos playerTurn siempre sera falso asi que se dibujaran sin glow

    foreach(QJsonValue value, Utility::getCardAtribute(code, "mechanics").toArray())
    {
        qDebug()<<value.toString();
        processTagChange(value.toString(), "1");
    }
}


int MinionGraphicsItem::getId()
{
    return this->id;
}


void MinionGraphicsItem::setPlayerTurn(bool playerTurn)
{
    this->playerTurn = playerTurn;
    update();
}


void MinionGraphicsItem::changeZone(bool playerTurn)
{
    this->friendly = !this->friendly;
    this->playerTurn = this->friendly && playerTurn;
    this->exausted = true;
}


QRectF MinionGraphicsItem::boundingRect() const
{
    //185x200 Minion Framework
    return QRectF( -142/2, -184/2, 142, 184);
}


void MinionGraphicsItem::setZonePos(bool friendly, int pos, int minionsZone)
{
    int x = 140*(pos - (minionsZone-1)/2.0);
    int y = friendly?180/2:-180/2;
    this->setPos(x, y);
}


void MinionGraphicsItem::processTagChange(QString tag, QString value)
{
    qDebug()<<"Graphic item"<<id<<tag<<value;
    if(tag == "DAMAGE")
    {
        this->damage = value.toInt();
    }
    else if(tag == "ATK")
    {
        this->attack = value.toInt();
    }
    else if(tag == "HEALTH")
    {
        this->health = value.toInt();
    }
    else if(tag == "EXHAUSTED")
    {
        if(friendly)    this->exausted = (value=="1");
    }
    else if(tag == "DIVINE_SHIELD")
    {
        this->shield = (value=="1");
    }
    else if(tag == "TAUNT")
    {
        this->taunt = (value=="1");
    }
    update();
}


void MinionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    //Card background
    painter->setBrush(QBrush(QPixmap(Utility::hscardsPath() + "/" + code + ".png")));
    painter->setBrushOrigin(QPointF(200-100,303-112));
    painter->drawEllipse(QPointF(0,0), 50, 68);

    //Taunt/Minion template
    if(this->taunt)
    {
        painter->drawPixmap(-140/2, -192/2, QPixmap(":Images/bgMinionTaunt" + QString((exausted || !playerTurn)?"Simple":"Glow") + ".png"));
        painter->drawPixmap(-140/2, -160/2, QPixmap(":Images/bgMinionSimple.png"));
    }
    else
    {
        painter->drawPixmap(-140/2, -160/2, QPixmap(":Images/bgMinion" + QString((exausted || !playerTurn)?"Simple":"Glow") + ".png"));
    }


    //Attack/Health
    QFont font("Belwe Bd BT");
    font.setPixelSize(45);
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
    font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
    font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif
    painter->setFont(font);
    painter->setBrush(WHITE);
    QPen pen(BLACK);
    pen.setWidth(2);
    painter->setPen(pen);

    if(attack>origAttack)   painter->setBrush(GREEN);
    else                    painter->setBrush(WHITE);
    QFontMetrics fm(font);
    int textWide = fm.width(QString::number(attack));
    int textHigh = fm.height();
    QPainterPath path;
    path.addText(-35 - textWide/2, 46 + textHigh/4, font, QString::number(attack));
    painter->drawPath(path);

    if(damage>0)                painter->setBrush(RED);
    else if(health>origHealth)  painter->setBrush(GREEN);
    else                        painter->setBrush(WHITE);
    textWide = fm.width(QString::number(health-damage));
    path = QPainterPath();
    path.addText(34 - textWide/2, 46 + textHigh/4, font, QString::number(health-damage));
    painter->drawPath(path);

    //Shield
    if(this->shield)
    {
        painter->drawPixmap(-142/2, -184/2, QPixmap(":Images/bgMinionShield.png"));
    }
}

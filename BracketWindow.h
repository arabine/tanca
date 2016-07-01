#ifndef BRACKETWINDOW_H
#define BRACKETWINDOW_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QGraphicsRectItem>
#include "DbManager.h"

class BaseRect : public QGraphicsRectItem
{
public:
    enum Rounded
    {
        NO_ROUND = 0,
        TOP_LEFT = 1,
        TOP_RIGHT = 2,
        BOTTOM_LEFT = 4,
        BOTTOM_RIGHT = 8
    };

    BaseRect(int rounded, QGraphicsItem *parent = 0)
        : QGraphicsRectItem(parent)
        , mRounded(rounded)
        , font("Arial", 10, QFont::Normal)
        , mTextColor(Qt::white)
    {

    }

    void SetText(const QString &s);
    void SetFont(const QFont &f);
    void SetTextColor(Qt::GlobalColor c);

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    int mRounded;

    QString text;
    QFont font;
    Qt::GlobalColor mTextColor;
};


class BracketBox : public QGraphicsItem
{
public:
    enum Position
    {
        TOP,
        BOTTOM
    };

    static const int cHeight = 20;
    static const int cWidth = 250;

    BracketBox(Position pos, QGraphicsItem *parent)
        : QGraphicsItem(parent)
        , mPosition(pos)
        , mLeft((pos == TOP) ? BaseRect::TOP_LEFT : BaseRect::BOTTOM_LEFT, this)
        , mMiddle(BaseRect::NO_ROUND, this)
        , mRight((pos == TOP) ? BaseRect::TOP_RIGHT : BaseRect::BOTTOM_RIGHT, this)
    {

        mLeft.setBrush(QColor(128, 128, 128));
        mLeft.setRect(0, 0, cHeight, cHeight);
        mLeft.SetTextColor(Qt::black);

        mMiddle.setBrush(QColor(98, 98, 98));
        mMiddle.setRect(cHeight, 0, cWidth, cHeight);

        mRight.setBrush(QColor(128, 128, 128));
        mRight.setRect(cHeight + cWidth, 0, cHeight, cHeight);
        mRight.SetTextColor(Qt::black);
    }

    void SetPlayerName(const QString &name)
    {
        mMiddle.SetText(name);
    }

    void SetScore(const int score)
    {
        if (score >= 0)
        {
            mRight.SetText(QString::number(score));
        }
        else
        {
            mRight.SetText("");
        }
    }

    void SetId(const int id)
    {
        mLeft.SetText(QString::number(id));
    }

protected:

    QRectF boundingRect() const
    {
        qreal width = mLeft.rect().width() + mMiddle.rect().width() + mRight.rect().width();
        return QRectF(QPointF(0,0), QSizeF(width, mLeft.rect().height()));
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

private:
    Position mPosition;

    // The bracket is composed by three rectangles:
    BaseRect mLeft;
    BaseRect mMiddle;
    BaseRect mRight;
 };


class MatchGroup
{
public:
    static const int cSpacer = 2;
    static const int cGroupHeight = 2 * BracketBox::cHeight + cSpacer;

    MatchGroup(QGraphicsScene *scene, QGraphicsItem *parent);

    void SetTeam(BracketBox::Position position, const Team &team);
    void SetScore(BracketBox::Position position, int score);
    void SetId(BracketBox::Position position, int id);
    void Move(const QPointF &origin);


private:
    BracketBox *boxTop;
    BracketBox *boxBottom;
};

class RoundBox : public BaseRect
{
public:

    static const int cHeight = 20;
    static const int cWidth = 300;

    RoundBox(int column)
        : BaseRect(NO_ROUND, NULL)
    {
        setBrush(QColor(62, 62, 62));
        setRect(0, 0, cWidth, cHeight);
        SetText(QObject::tr("Partie %1").arg(column));
    }



    ~RoundBox()
    {
        qDeleteAll(mList.begin(), mList.end());
        mList.clear();
    }

    void AddMatch(MatchGroup *match);

private:
    QList<MatchGroup *> mList;

};

class View : public QGraphicsView
{

public:

    View(QGraphicsScene *scene, QWidget *parent);

    void resizeEvent(QResizeEvent *event);
};


class Scene : public QGraphicsScene
{

public:

    Scene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0)
        : QGraphicsScene(x, y, width, height, parent)
    {

    }

};


class BracketWindow : public QDialog
{
    Q_OBJECT
public:
    explicit BracketWindow(QWidget *parent = 0);

    QList<Game> BuildRounds(const QList<Team> &tlist);
    void SetGames(const QList<Game> &games, const QList<Team> &teams);

private:
    Scene *mScene;
    View *mView;
    QList<Game> mGames;
    QList<RoundBox *> mBoxes;
    int mTurns;
};

#endif // ROUNDWINDOW_H

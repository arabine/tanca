#ifndef BRACKETWINDOW_H
#define BRACKETWINDOW_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include "ui_RoundWindow.h"

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

    static const qreal cHeight = 20.0f;
    static const qreal cWidth = 250.0f;

    BracketBox(Position pos)
        : mColumn(0)
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
    int mColumn;
    Position mPosition;

    // The bracket is composed by three rectangles:
    BaseRect mLeft;
    BaseRect mMiddle;
    BaseRect mRight;
 };


class MatchGroup
{
public:
    MatchGroup(QGraphicsScene *scene);

private:
    BracketBox *boxTop;
    BracketBox *boxBottom;
};

class RoundBox : public BaseRect
{
public:

    static const qreal cHeight = 20.0f;
    static const qreal cWidth = 300.0f;

    RoundBox(int column)
        : BaseRect(0, this)
    {
        setBrush(QColor(62, 62, 62));
        setRect(0, 0, cWidth, cHeight);
        SetText(QObject::tr("Partie %1").arg(column));
    }

    void AddMatch(MatchGroup *match) { mList.append(match); }

private:
    QList<MatchGroup *> mList;

};

class View : public QGraphicsView
{

public:

    View(QGraphicsScene *scene, QWidget *parent);

    void resizeEvent(QResizeEvent *event);
};



class BracketWindow : public QDialog
{
    Q_OBJECT
public:
    explicit BracketWindow(QWidget *parent = 0);

private:
    QGraphicsScene *mScene;
    View *mView;
};

#endif // ROUNDWINDOW_H

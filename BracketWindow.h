#ifndef ROUNDWINDOW_H
#define ROUNDWINDOW_H

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
    {

    }

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        // Paint with specified color and pen
        painter->setRenderHint(QPainter::Antialiasing);

        painter->setPen(Qt::NoPen);
        painter->setBrush(brush());

        qreal top = rect().top();
        qreal width = rect().width();
        qreal height = rect().height();
        qreal left = rect().left();

        QPainterPath path;
        path.setFillRule( Qt::WindingFill );

        path.addRoundedRect(rect(), 5, 5 );
        qreal squareSize = height/2;

        // Draw rectangles where there is no any rounded corner
        if (!(mRounded & BOTTOM_LEFT))
        {
            path.addRect( QRectF( left, top + height-squareSize, squareSize, squareSize) ); // Bottom left
        }

        if (!(mRounded & BOTTOM_RIGHT))
        {
            path.addRect( QRectF( (left+width)-squareSize, top+height-squareSize, squareSize, squareSize) ); // Bottom right
        }

        if (!(mRounded & TOP_LEFT))
        {
            path.addRect( QRectF( left, top, squareSize, squareSize) ); // top left
        }

        if (!(mRounded & TOP_RIGHT))
        {
            path.addRect( QRectF( (left+width)-squareSize, top, squareSize, squareSize) ); // top right
        }

        painter->drawPath( path.simplified() ); // Draw box (only rounded at top)
    }

private:
    int mRounded;

};


class BracketBox : public QGraphicsItem
{
public:
    enum Position
    {
        TOP,
        BOTTOM
    };

    static const qreal cHeight = 40.0f;
    static const qreal cWidth = 150.0f;

    BracketBox(Position pos)
        : mColumn(0)
        , mPosition(pos)
        , mLeft(BaseRect::TOP_LEFT, this)
        , mMiddle(BaseRect::NO_ROUND, this)
        , mRight(BaseRect::TOP_RIGHT, this)
    {

        mLeft.setBrush(QColor(128, 128, 128));
        mLeft.setRect(0, 0, cHeight, cHeight);

        mMiddle.setBrush(QColor(98, 98, 98));
        mMiddle.setRect(cHeight, 0, cWidth, cHeight);

        mRight.setBrush(QColor(128, 128, 128));
        mRight.setRect(cHeight + cWidth, 0, cHeight, cHeight);
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


class RoundWindow : public QDialog
{
    Q_OBJECT
public:
    explicit RoundWindow(QWidget *parent = 0);

private:
        Ui::RoundWindow ui;
        QGraphicsScene mScene;
        QList<BracketBox*> mList;
};

#endif // ROUNDWINDOW_H

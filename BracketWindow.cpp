#include "BracketWindow.h"

#include <QRadialGradient>

void BaseRect::SetText(const QString &s)
{
    text = s;
    text.truncate(30);
}

void BaseRect::SetFont(const QFont &f)
{
    font = f;
}

void BaseRect::SetTextColor(Qt::GlobalColor c)
{
    mTextColor = c;
}

void BaseRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

    // Text inside the box
    painter->save();
    if (text.size() > 0)
    {
        painter->setPen(mTextColor);
        painter->setFont(font);
        painter->drawText(rect(), Qt::AlignCenter, text);
    }
    painter->restore();
}


/*****************************************************************************/

View::View(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{

}

void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

BracketWindow::BracketWindow(QWidget *parent)
    : QDialog(parent)
{
    mScene = new QGraphicsScene(0, 0, 400, 320, this);
    mView = new View(mScene, this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mView);

    setLayout(mainLayout);
    resize(400, 320);

    // a blue background
    mScene->setBackgroundBrush(QColor(68,68,68));

    MatchGroup *match = new MatchGroup(mScene);
    RoundBox *roundBox1 = new RoundBox(1);
    roundBox1->AddMatch(match);

    mScene->addItem(roundBox1);

}

MatchGroup::MatchGroup(QGraphicsScene *scene)
{
    boxTop = new BracketBox(BracketBox::TOP);
    boxBottom = new BracketBox(BracketBox::BOTTOM);
    scene->addItem(boxTop);
    scene->addItem(boxBottom);

    boxTop->setPos(10, 20);
    boxBottom->setPos(10, BracketBox::cHeight + 22);

    boxTop->SetPlayerName("Anthony Rabine / Kévin Machin");


}

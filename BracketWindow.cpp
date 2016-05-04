#include "BracketWindow.h"

#include <QVBoxLayout>
#include <QRadialGradient>
#include <QMessageBox>
#include <iostream>

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
    (void) event;
 //   QGraphicsView::resizeEvent(event);
 //   fitInView(sceneRect(), Qt::KeepAspectRatio);
}

/*****************************************************************************/

MatchGroup::MatchGroup(QGraphicsScene *scene, QGraphicsItem *parent)
{
    boxTop = new BracketBox(BracketBox::TOP, parent);
    boxBottom = new BracketBox(BracketBox::BOTTOM, parent);

    Move(QPointF(0, 0));

    scene->addItem(boxTop);
    scene->addItem(boxBottom);
}

void MatchGroup::SetTeam(BracketBox::Position position, const Team &team)
{
    if (position == BracketBox::TOP)
    {
        boxTop->SetPlayerName(team.teamName);
    }
    else
    {
        boxBottom->SetPlayerName(team.teamName);
    }
}

void MatchGroup::Move(const QPointF &origin)
{
    boxTop->setPos(origin);
    boxBottom->setPos(origin.x(), origin.y() + BracketBox::cHeight + cSpacer); // 2 pixels between boxes
}

/*****************************************************************************/

BracketWindow::BracketWindow(QWidget *parent)
    : QDialog(parent, Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
    , mRounds(3)
{
    mScene = new Scene(0, 0, 950, 400, this);
    mView = new View(mScene, this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mView);

    setLayout(mainLayout);

    // a blue background
    mScene->setBackgroundBrush(QColor(68,68,68));
}

bool BracketWindow::SetTeams(const QList<Team> &teams)
{
    bool ok = false;
    mTeams = teams;

    qDeleteAll(mBoxes.begin(), mBoxes.end());
    mBoxes.clear();

    for (int i = 0; i < mTeams.size(); i++)
    {
        mTeams[i].opponents.clear();
    }

    // Check if there is enough teams for the desired number of rounds
    if (mTeams.size() > mRounds)
    {
        for (int i = 0; i < mRounds; i++)
        {
            RoundBox *roundBox = new RoundBox(i+1);
            QPointF pos;

            pos.setX(20 + (i * RoundBox::cWidth));
            pos.setY(0);

            roundBox->setPos(pos);
            mBoxes.append(roundBox);
            mScene->addItem(roundBox);

            // Create matches for all teams
            for (int j = 0; j < mTeams.size(); j++)
            {
                Team &team = mTeams[j];

                // Test if we have been already assigned to a match
                if (team.opponents.size() != (i+1))
                {
                    int k;
                    // Find opponents for that round
                    for (k = 0; k < mTeams.size(); k++)
                    {
                        Team &opp = mTeams[k];
                        if ((opp.id != team.id) &&
                            (!team.opponents.contains(opp.id)) &&
                            (!opp.opponents.contains(team.id)) &&
                            (opp.opponents.size() != (i+1)))
                        {
                            // Found free oponent, memorize it for that round
                            team.opponents.append(opp.id);
                            opp.opponents.append(team.id);
                            break;
                        }
                    }

                    if (k != mTeams.size())
                    {
                        Team &opp = mTeams[k];
                        std::cout << "Round: " << (i+1) << ", Match: " << team.teamName.toStdString() << " <- vs -> " << opp.teamName.toStdString() << std::endl;

                        MatchGroup *match = new MatchGroup(mScene, roundBox);
                        match->SetTeam(BracketBox::TOP, team);
                        match->SetTeam(BracketBox::BOTTOM, opp);

                        roundBox->AddMatch(match);
                    }
                    else
                    {
                        std::cout << "Cannot find opponent!" << std::endl;
                    }
                }
            } // end for rounds
        }

        ok = true;
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Il n'y a pas assez d'équipes pour jouer %1 parties.").arg(mRounds),
                                    QMessageBox::Ok);
    }

    return ok;
}


void RoundBox::AddMatch(MatchGroup *match)
{
    int offset = mList.size() * (MatchGroup::cGroupHeight + (MatchGroup::cSpacer *4)); // bigger spacer between match groups

    QPointF round = pos();

    round.setX(10);
    round.setY(round.y() + offset + RoundBox::cHeight + (MatchGroup::cSpacer * 4));

    match->Move(round);

    mList.append(match);
}

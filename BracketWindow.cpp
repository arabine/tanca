#include "BracketWindow.h"

#include <QVBoxLayout>
#include <QRadialGradient>
#include <QMessageBox>
#include <iostream>
#include <algorithm>

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

   // scene->addItem(boxTop);
  //  scene->addItem(boxBottom);
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
    , mTurns(3)
{
    mScene = new Scene(0, 0, 950, 400, this);
    mView = new View(mScene, this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mView);

    setLayout(mainLayout);

    // a blue background
    mScene->setBackgroundBrush(QColor(68,68,68));
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

/*****************************************************************************/

// Return true if there is no round assigned to this team for this turn
bool BracketWindow::IsFree(const int id, const int turn)
{
    bool isFree = true;

    for (int i = 0; i < mGames.size(); i++)
    {
        const Game &round = mGames.at(i);
        if (round.turn == turn)
        {
            if ((round.team1Id == id) ||
                (round.team2Id == id))
            {
                isFree = false;
            }
        }
    }
    return isFree;
}

bool BracketWindow::HasAlreadyPlayed(const int id1, const int id2)
{
    bool alreadyPlayed = false;

    for (int i = 0; i < mGames.size(); i++)
    {
        const Game &round = mGames.at(i);
        if (((round.team1Id == id1) &&
             (round.team2Id == id2)) ||
            ((round.team1Id == id2) &&
             (round.team2Id == id1)))
        {
            alreadyPlayed = true;
        }
    }
    return alreadyPlayed;
}


int BracketWindow::Randomize(const Team &team, int turn)
{
    bool found = false;
    int k = -1;

    do
    {
        // Find opponents for that round
        k = qrand()%mTeams.size();
    //    std::cout << "Trying: " << k << std::endl;
        Team &opp = mTeams[k];
        if ((opp.id != team.id) &&
            IsFree(opp.id, turn) &&
            !HasAlreadyPlayed(opp.id, team.id))
        {
            // Found free oponent, memorize it for that round
            found = true;
        }
    }
    while(!found);

    return k;
}


QList<Game> BracketWindow::BuildRounds(const QList<Team> &teams)
{
    mTeams = teams;
    mGames.clear();

    // Check if there is enough teams for the desired number of rounds
    if (mTeams.size() > mTurns)
    {
        for (int i = 0; i < mTurns; i++)
        {
            // Create fuzzy, never start with the same team
            std::random_shuffle(mTeams.begin(), mTeams.end());

            // Don't manage odd number of teams
            int max_games = mTeams.size() / 2;
            int games = 0;

            // Create matches for all teams
            for (int j = 0; j < mTeams.size(); j++)
            {
                Team &team = mTeams[j];

                // Test if we have been already assigned to a match
                if (IsFree(team.id, i))
                {
                    int k = Randomize(team, i);

                    // Valid team id, create the round
                    if ((k != mTeams.size()) && (k >= 0))
                    {
                        Team &opp = mTeams[k];
                        Game game;

                        game.eventId = opp.eventId;
                        game.turn = i;
                        game.team1Id = team.id;
                        game.team2Id = opp.id;

                        mGames.append(game);

                        games++;

                        std::cout << "Turn: " << (i+1) << ", Game: " << team.teamName.toStdString() << " <- vs -> " << opp.teamName.toStdString() << std::endl;
                    }
                    else
                    {
                        std::cout << "Cannot find opponent!" << std::endl;
                    }
                }

                // Exit if we reachs the number of games
                // We will artificially exit when there is an odd number of teams
                if (games >= max_games)
                {
                    break;
                }
            }
        }
    }
    else
    {
        (void) QMessageBox::warning(this, tr("Tanca"),
                                    tr("Il n'y a pas assez d'équipes pour jouer %1 parties.").arg(mTurns),
                                    QMessageBox::Ok);
    }

    return mGames;
}


bool BracketWindow::FindTeam(const int id, Team &team)
{
    bool found = false;
    for (int i = 0; i < mTeams.size(); i++)
    {
        if (mTeams[i].id == id)
        {
            found = true;
            team = mTeams[i];
            break;
        }
    }

    return found;
}

void BracketWindow::SetGames(const QList<Game>& games)
{
    mGames = games;
    qDeleteAll(mBoxes.begin(), mBoxes.end());
    mBoxes.clear();

    for (int i = 0; i < mTurns; i++)
    {
        RoundBox *roundBox = new RoundBox(i+1);
        QPointF pos;

        pos.setX(20 + (i * RoundBox::cWidth));
        pos.setY(0);

        roundBox->setPos(pos);
        mBoxes.append(roundBox);
        mScene->addItem(roundBox);

        // Create matche boxes for that turn
        for (int j = 0; j < mGames.size(); j++)
        {
            const Game &round = mGames.at(j);
            if (round.turn == i)
            {
                MatchGroup *match = new MatchGroup(mScene, roundBox);

                Team team;
                if (FindTeam(round.team1Id, team))
                {
                    match->SetTeam(BracketBox::TOP, team);
                }
                else
                {
                    std::cout << "Impossible de trouver l'équipe !" << std::endl;
                }

                if (FindTeam(round.team2Id, team))
                {
                    match->SetTeam(BracketBox::BOTTOM, team);
                }
                else
                {
                    std::cout << "Impossible de trouver l'équipe !" << std::endl;
                }

                roundBox->AddMatch(match);
            }
        }
    }

}


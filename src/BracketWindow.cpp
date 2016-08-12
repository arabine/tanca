#include "BracketWindow.h"

#include <QVBoxLayout>
#include <QRadialGradient>
#include <QMessageBox>
#include <iostream>
#include <algorithm>
#include "Log.h"

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
    Q_UNUSED(scene);
    boxTop = new BracketBox(BracketBox::TOP, parent);
    boxBottom = new BracketBox(BracketBox::BOTTOM, parent);

    Move(QPointF(0, 0));
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

void MatchGroup::SetScore(BracketBox::Position position, int score)
{
    if (position == BracketBox::TOP)
    {
        boxTop->SetScore(score);
    }
    else
    {
        boxBottom->SetScore(score);
    }
}

void MatchGroup::SetId(BracketBox::Position position, int id)
{
    if (position == BracketBox::TOP)
    {
        boxTop->SetId(id);
    }
    else
    {
        boxBottom->SetId(id);
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
QList<Game> BracketWindow::BuildRounds(const QList<Team> &tlist)
{
    mGames.clear();
    QList<Team> teams = tlist; // Local copy to manipulate the list

    if (teams.size()%2)
    {
        // odd numeber of team, add dummy one
        Team dummy;
        teams.append(dummy);
    }

    int max_games = teams.size() / 2;

    // Create fuzzy, never start with the same team
    std::random_shuffle(teams.begin(), teams.end());

    // Check if there is enough teams for the desired number of rounds
    // Swiss tournament algorithm
    if (teams.size() > mTurns)
    {
        for (int i = 0; i < mTurns; i++)
        {
            // Create matches for this turn
            for (int j = 0; j < max_games; j++)
            {
                const Team &team = teams[j];
                const Team &opp = teams[teams.size() - j - 1];

                Game game;

                game.eventId = opp.eventId;
                game.turn = i;
                game.team1Id = team.id;
                game.team2Id = opp.id;

                mGames.append(game);

                std::cout << "Turn: " << (i+1) << ", Game: " << team.teamName.toStdString() << " <- vs -> " << opp.teamName.toStdString() << std::endl;
            }

            // Then rotate, keep first player always at the first place
            Team first = teams.takeFirst();
            Team last = teams.takeLast();
            teams.prepend(last);
            teams.prepend(first);
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


void BracketWindow::SetGames(const QList<Game>& games, const QList<Team> &teams)
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

                // Be tolerant: only show found teams
                Team team;
                if (Team::Find(teams, round.team1Id, team))
                {
                    match->SetTeam(BracketBox::TOP, team);
                    if (team.number > 0)
                    {
                        match->SetId(BracketBox::TOP, team.number);
                    }
                    match->SetScore(BracketBox::TOP, round.team1Score);
                }

                if (Team::Find(teams, round.team2Id, team))
                {
                    match->SetTeam(BracketBox::BOTTOM, team);
                    if (team.number > 0)
                    {
                        match->SetId(BracketBox::BOTTOM, team.number);
                    }
                    match->SetScore(BracketBox::BOTTOM, round.team2Score);
                }

                roundBox->AddMatch(match);
            }
        }
    }

}


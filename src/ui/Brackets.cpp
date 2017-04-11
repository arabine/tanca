#include "Brackets.h"

Brackets::Brackets(QObject *parent)
    : QObject(parent)
    , mRounds("[]")
{

}

QString Brackets::getRounds()
{
    return mRounds;
}

void Brackets::setRounds(const QString &rounds)
{
    mRounds = rounds;
    emit roundsChanged();
}

#ifndef BRACKETS_H
#define BRACKETS_H

#include <QObject>

class Brackets : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString rounds READ getRounds WRITE setRounds NOTIFY roundsChanged)

public:
    explicit Brackets(QObject *parent = 0);

    QString getRounds();
    void setRounds(const QString &rounds);

signals:
  void roundsChanged();

public slots:

private:
    QString mRounds;
};

#endif // BRACKETS_H

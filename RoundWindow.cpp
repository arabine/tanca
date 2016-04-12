#include "RoundWindow.h"

#include <QRadialGradient>

RoundWindow::RoundWindow(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);

    ui.graphicsView->setScene(&mScene);


    // a blue background
    mScene.setBackgroundBrush(QColor(68,68,68));

    BracketBox *box = new BracketBox(BracketBox::TOP);
    mScene.addItem(box);
    box->setPos(20,20);

}

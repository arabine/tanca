/**
 *  Tanca, a petanque contests manager
 *  Copyright (C) 2016  Anthony Rabine <anthony.rabine@tarotclub.fr>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include <QApplication>
#include <QtQuick>

#include <iostream>
#include <fstream>
/*
#include <chrono>
#include <thread>
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
*/
#include "Brackets.h"
#include "Tournament.h"
#include "Log.h"

//#define UNIT_TESTS


/*****************************************************************************/
class Logger : public Observer<std::string>
{
public:
    Logger()
        : Observer(0xFFU)
    {

    }

    void Update(const std::string &info)
    {
        std::cout << info << std::endl;
    }
};

extern void RunTests();

int main(int argc, char *argv[])
{

#ifdef UNIT_TESTS

    (void) argc;
    (void) argv;

    RunTests();
    return 0;
#else
    Logger logger;
    Log::RegisterListener(logger);

    QApplication a(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<Brackets>("Tanca", 1, 0, "Brackets");

    MainWindow w;
    w.Initialize();
    w.show();

    return a.exec();
#endif

}

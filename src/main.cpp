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

#include <iostream>
#include <fstream>
/*
#include <chrono>
#include <thread>
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
*/
#include "Tournament.h"
#include "Log.h"

//#define UNIT_TESTS


/*****************************************************************************/
class Logger : public Observer<Log::Infos>
{
public:
    Logger()
        : Observer(0xFFU)
    {

    }

    virtual ~Logger();

    void Update(const Log::Infos &info)
    {
        std::cout << info.message << std::endl;
    }
};

extern void RunTests();

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    Logger logger;
    Log::RegisterListener(logger);

    QApplication a(argc, argv);

    MainWindow w;
    w.Initialize();
    w.show();

    return a.exec();

}

Logger::~Logger() {}

/*
 * EasyIRCRelay
 * Copyright 2016 (c) - Maxwell Dreytser
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <QCoreApplication>
#include <iostream>

#include "mainworker.h"

void PrintSyntax(char *argv[]) {
    std::cerr << argv[0];
    std::cerr << " [Config File Path]" << std::endl;
    std::cerr << "Note that if no config path is specified, I will look in my working directory." << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MainWorker* Worker = new MainWorker();
    switch (argc) {
    case 1:
        Worker->StartMainWorker();
        break;
    case 2:
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-h") == 0) {
            PrintSyntax(argv);
            return 0;
        }
        Worker->StartMainWorker(QString(argv[1]));
        break;
    default:
        std::cerr << "Too many arguments." << std::endl;
        PrintSyntax(argv);
        exit(1);
        break;
    }

    return a.exec();
}

/* Muistipelin paluu
 *
 * Kuvaus:
 *  Ohjelma toteuttaa GUI-version muistipelistä, joka on toteutettu
 * QT Widgets Application:na. Tarkempi kuvaus ohjelman toiminnasta ja
 * käytöstä löytyy tiedostosta instructions.txt.
 *
 */

#include "mainwindow.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

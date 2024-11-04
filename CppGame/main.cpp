#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include "Board.h"
#include "Score.h"
#include "Values.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(QApplication::desktop()->geometry());

    Values values;
    values.score.set(0);


    Board board(&scene, &values);
    Score score(&scene, &values);

    QGraphicsView view(&scene);
    view.show();

    return a.exec();
}
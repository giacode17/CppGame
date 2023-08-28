
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene> //Scene
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
//#include <QtWidgets/QGraphicsPixmapItem> //Call a image from the computer, fix the image to scene
//#include "Consts.h"
#include "Board.h"
 
int main(int argc, char *argv[])
{
    QApplication a(argc, argv); //application
    QRect geometry = QApplication::desktop()->geometry(); //to get current desktop's size :1

    QGraphicsScene scene;
    scene.setSceneRect(geometry);// from 1, we can set the coordinate 0,0 to left upper corner.
   
    Board board(&scene);
    
    QGraphicsView view(&scene); //To view the result.
    view.show();//To view the result.

   
    return a.exec();
}





//

/*< Add rectangle item on screen>
    QGraphicsRectItem rect(0, 0, 1000, 1000); //x, y, width, heigth
    scene.addItem(&rect); //add 'rect' item with on scence with pointer but, can not see a resutl, just work
    */

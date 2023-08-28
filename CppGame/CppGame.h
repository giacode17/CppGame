#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CppGame.h"

class CppGame : public QMainWindow
{
    Q_OBJECT

public:
    CppGame(QWidget *parent = nullptr);
    ~CppGame();

private:
    Ui::CppGameClass ui;
};

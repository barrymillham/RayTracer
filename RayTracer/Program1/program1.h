#ifndef PROGRAM1_H
#define PROGRAM1_H

#include <QtWidgets/QMainWindow>
#include "ui_program1.h"

class Program1 : public QMainWindow
{
	Q_OBJECT

public:
	Program1(QWidget *parent = 0);
	~Program1();

private:
	Ui::Program1Class ui;
};

#endif // PROGRAM1_H

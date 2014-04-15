/**********************************************************
  Author: Ethan Johnson
  Course: COMP 361, Introduction to Computer Graphics
  Assignment: Program 4
  Date: 4/2/2014
  Description: Adds a Mesh class to Program 3, and uses it to render
	geometry generated with extrusions and surfrevs.
**********************************************************/

#pragma once

#include <QLineEdit>

class MyLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	MyLineEdit(QWidget* parent) : QLineEdit(parent)
	{}
	~MyLineEdit(void) {}

signals:
	void textOut(QString);

public slots:
	void sendText()
	{
		QString textContents = text();
		emit textOut(textContents);
	}
};
#pragma once

#include <QWidget>
#include "ui_QProgressWidget.h"

class QProgressWidget : public QWidget
{
	Q_OBJECT

public:
	QProgressWidget(QWidget *parent = Q_NULLPTR);
	~QProgressWidget();
	void SetValue(int);
private:
	Ui::QProgressWidget ui;
protected slots:
		
};

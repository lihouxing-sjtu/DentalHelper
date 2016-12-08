#include "stdafx.h"
#include "QProgressWidget.h"

QProgressWidget::QProgressWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

QProgressWidget::~QProgressWidget()
{
}

void QProgressWidget::SetValue(int p)
{
	ui.progressBar->setValue(p);
}

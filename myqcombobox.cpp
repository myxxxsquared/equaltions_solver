#include "stdafx.h"
#include "myqcombobox.h"

QMyComboBox::QMyComboBox(QWidget *parent,int index)
	: QComboBox(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(OnTriggered()));
	this->index = index;
}

QMyComboBox::~QMyComboBox()
{

}

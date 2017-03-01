#ifndef MYQCOMBOBOX_H
#define MYQCOMBOBOX_H


class QMyComboBox : public QComboBox
{
	Q_WIDGET

public:
	QMyComboBox(QObject *parent,int index=0);
	~QMyComboBox();
	int index;
signals:
	void MyQComboBoxtriggered(QComboBox*);
private slots:
    void OnTriggered();


	
};

#endif // MYQCOMBOBOX_H

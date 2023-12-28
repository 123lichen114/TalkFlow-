#ifndef QTSOCKETCLIENT_H
#define QTSOCKETCLIENT_H

#include <QtWidgets/QWidget>
#include "ui_client.h"

#include <QtNetwork>
#include <QHostAddress>
#include <QMessageBox>
#include <QDataStream>
#include <QByteArray>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>

class Client : public QWidget
{
	Q_OBJECT

public:
	Client(QWidget *parent = 0);
	~Client();

	void initTCP();
	void newConnect();

	private slots:
	// ���ӷ�����
	void connectServer();
	// �Ͽ�����
	void disconnectServer();
	// ���շ��������͵�����
	void receiveData();
	// ����������������
	void sendData();

	// �����ļ�
	void selectFile();
	// �����ļ�
	void sendFile();
	// �����ļ����ͽ���
	void updateFileProgress(qint64);
	// �����ļ����ս���
	void updateFileProgress();

    void on_pushButton_connect_clicked();

private:
	Ui::ClientClass ui;

	QTcpSocket *tcpSocket;
	QTcpSocket *fileSocket;

	// �ļ�����
	QFile *localFile;
	// �ļ���С
	qint64 totalBytes;      //�ļ����ֽ���
	qint64 bytesWritten;    //�ѷ��͵��ֽ���
	qint64 bytestoWrite;    //��δ���͵��ֽ���
	qint64 filenameSize;    //�ļ����ֵ��ֽ���
	qint64 bytesReceived;   //���յ��ֽ���
	// ÿ�η������ݴ�С
	qint64 perDataSize;
	QString filename;
	// ���ݻ�����
	QByteArray inBlock;
	QByteArray outBlock;

	// ϵͳʱ��
	QDateTime current_date_time;
	QString str_date_time;
};

#endif // QTSOCKETCLIENT_H

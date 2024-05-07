#include <QtTest/QTest>
#include <QProcess>

#include <iostream>

class ProcessHandler : public QObject
{
	Q_OBJECT
public:
	ProcessHandler(QProcess& process)
		: m_process{ process }
	{
	}

public slots:
	void readData()
	{
		QString str = m_process.readAllStandardOutput();
		std::cout << str.toStdString() << std::endl;
	}

private:
	QProcess& m_process;
};

class TestProcessPipe : public QObject
{
	Q_OBJECT
private slots:
	void run();
};

void TestProcessPipe::run()
{
	QProcess cmd{ this };
	ProcessHandler handler{ cmd };

	QObject::connect(&cmd, &QProcess::readyReadStandardOutput, &handler, &ProcessHandler::readData);

	cmd.startCommand("ping -n 10 google.com");
	cmd.waitForFinished();
}

QTEST_MAIN(TestProcessPipe)
#include "main.moc"

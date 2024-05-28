#pragma once

#include <memory>

#include <QObject>

#include <rconpp/rcon.h>

class RCONClient : public QObject
{
	Q_OBJECT
public:
	explicit RCONClient(QObject* parent = nullptr);

public:
	void Exec(const QString& cmd);
	void Reset();

private:
	std::unique_ptr<rconpp::rcon_client> m_client;
};

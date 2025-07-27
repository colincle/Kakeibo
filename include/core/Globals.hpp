#pragma once

#include "EnveloppeManager.hpp"

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

inline EnveloppeManager g_enveloppeManager;

float getEurJpyRateBlocking();

inline float getEurJpyRateCached()
{
	static float rate = getEurJpyRateBlocking();
	return rate;
}

inline float getEurJpyRateBlocking()
{
	QNetworkAccessManager manager;
	QEventLoop            loop;
	QString               apiKey = "REMOVED_API_KEY";
	QUrl                  url("http://api.currencylayer.com/live?access_key=" + apiKey + "&currencies=EUR,JPY");
	QNetworkReply        *reply = manager.get(QNetworkRequest(url));
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	float rate = -1.0f;

	if ( reply->error() == QNetworkReply::NoError )
	{
		QByteArray    data = reply->readAll();
		QJsonDocument doc  = QJsonDocument::fromJson(data);

		if ( doc.isObject() && doc.object()["success"].toBool() )
		{
			QJsonObject quotes   = doc.object()["quotes"].toObject();
			double      usdToJpy = quotes["USDJPY"].toDouble();
			double      usdToEur = quotes["USDEUR"].toDouble();
			rate                 = static_cast<float>(usdToJpy / usdToEur);
		}
		else
			qWarning() << "Invalid JSON or request failed";
	}
	else
		qWarning() << "Network error:" << reply->errorString();

	reply->deleteLater();
	return rate;
}

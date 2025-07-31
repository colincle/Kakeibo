#pragma once

#include "EnveloppeManager.hpp"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

inline EnveloppeManager g_enveloppeManager;

float getEurJpyRateBlocking();
float fallbackRate();

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

	if (rate == -1.0f)
		rate = fallbackRate();
	return rate;
}

inline float fallbackRate()
{
	FILE* pipe = popen(R"(curl -s "https://wise.com/gb/currency-converter/eur-to-jpy-rate")", "r");
	if (!pipe) return -1.0f;

	char buffer[4096];
	std::string html;

	while (fgets(buffer, sizeof(buffer), pipe))
		html += buffer;

	pclose(pipe);

	std::string markerStart = ">€1 EUR = ";
	std::string markerEnd = " JPY<";

	auto start = html.find(markerStart);
	if (start == std::string::npos) return -1.0f;
	start += markerStart.length();

	auto end = html.find(markerEnd, start);
	if (end == std::string::npos) return -1.0f;

	std::string rateStr = html.substr(start, end - start);
	std::stringstream ss(rateStr);
	float rate = -1.0f;
	ss >> rate;
	if (ss.fail()) return -1.0f;
	return rate;
}


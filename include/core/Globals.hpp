#pragma once

#include "EnvelopeManager.hpp"

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

inline EnvelopeManager g_envelopeManager;

float getEurJpyRateBlocking();

inline float getEurJpyRateCached()
{
	static float rate = getEurJpyRateBlocking();
	return rate;
}

// Money formatting helpers, so every page formats yen/euro the same way.
namespace Money
{
// Yen amount with Japanese thousands separators (no ¥ sign).
inline QString yen(int amount)
{
	return QLocale(QLocale::Japanese, QLocale::Japan).toString(amount);
}

// Euro value converted from yen (no € sign), or the given fallback if the
// exchange rate is unavailable.
inline QString euro(int amountYen, const QString &fallback = QStringLiteral("-"))
{
	float rate = getEurJpyRateCached();

	if ( rate > 0 )
		return QLocale(QLocale::French, QLocale::France).toString(amountYen / rate, 'f', 2);

	return fallback;
}
} // namespace Money

// Blocking GET of a keyless FX endpoint returning { "rates": { "JPY": <value> } }.
// Returns the EUR->JPY rate, or -1 on any failure.
inline float fetchEurJpy(const QUrl &url)
{
	QNetworkAccessManager manager;
	QEventLoop            loop;
	QNetworkReply        *reply = manager.get(QNetworkRequest(url));
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	float rate = -1.0f;

	if ( reply->error() == QNetworkReply::NoError )
	{
		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
		double        jpy = doc.object()["rates"].toObject()["JPY"].toDouble();

		if ( jpy > 0.0 )
			rate = static_cast<float>(jpy);
		else
			qWarning() << "Unexpected exchange-rate response from" << url.toString();
	}
	else
		qWarning() << "Network error:" << reply->errorString();

	reply->deleteLater();
	return rate;
}

inline float getEurJpyRateBlocking()
{
	// Primary and fallback are both keyless and share the same JSON shape.
	float rate = fetchEurJpy(QUrl("https://api.frankfurter.dev/v1/latest?base=EUR&symbols=JPY"));

	if ( rate <= 0.0f )
		rate = fetchEurJpy(QUrl("https://open.er-api.com/v6/latest/EUR"));

	return rate;
}

#include "request.h"

Request::Request(const QUrl & url)
    :QNetworkRequest(url)
{

setRawHeader("Content-Type","application/json;charset=UTF-8");
}

Request::Request(const Request &other)
    :QNetworkRequest(other)
{

setVerb(other.verb());
setParams(other.params());

}

Request::Request(const QNetworkRequest &other)
    :QNetworkRequest(other)
{

}

Request::~Request()
{

}


const QString &Request::verb() const
{
    return  mVerb;
}

void Request::setVerb(const QString &verb)
{
    mVerb = verb;

}

const QHash<QString, QVariant> &Request::params() const
{
    return mParams;

}

 QVariant Request::param(const QString &key) const
{
        return mParams.value(key);
}

void Request::insertParam(const QString &key, const QVariant &value)
{
    mParams.insert(key,value);
}

void Request::setParams(const QHash<QString, QVariant> &params)
{
    mParams = params;
}




#include "Mime.hpp"

Mime::Mime()
{
	mimeType["default"] = "text/html";
    mimeType["html"] = "text/html";
    mimeType["htm"] = "text/html";
    mimeType["txt"] = "text/plain";
    mimeType["css"] = "text/css";
    mimeType["ico"] = "image/x-icon";
    mimeType["jpg"] = "image/jpeg";
    mimeType["jpeg"] = "image/jpeg";
    mimeType["ico"] = "image/x-icon";
    mimeType["bmp"] = "image/bmp";
	mimeType["gif"] = "image/gif";
    mimeType["png"] = "image/png";
    mimeType["avi"] = "video/x-msvideo";
	mimeType["mp4"] = "video/mp4";
    mimeType["doc"] = "application/msword";
    mimeType["gz"] = "application/x-gzip";
    mimeType["xml"] = "application/xml";
    mimeType["pdf"] = "application/pdf";
    mimeType["mp3"] = "audio/mpeg";
    mimeType["wav"] = "audio/wav";
    mimeType["php"] = "application/x-httpd-php";
    mimeType["py"] = "application/x-httpd-php";
}

Mime::~Mime() {}

std::string Mime::getMimeType(std::string extn)
{
    if (mimeType.count(extn))
        return (mimeType[extn]);
    return (mimeType["default"]);
}

#include "Mime.hpp"

Mime::Mime()
{
	mime_types["default"] = "text/html";
    mime_types[".html"] = "text/html";
    mime_types[".htm"] = "text/html";
    mime_types[".txt"] = "text/plain";
    mime_types[".css"] = "text/css";
    mime_types[".ico"] = "image/x-icon";
    mime_types[".jpg"] = "image/jpeg";
    mime_types[".jpeg"] = "image/jpeg";
    mime_types[".ico"] = "image/x-icon";
    mime_types[".bmp"] = "image/bmp";
	mime_types[".gif"] = "image/gif";
    mime_types[".png"] = "image/png";
    mime_types[".avi"] = "video/x-msvideo";
	mime_types[".mp4"] = "video/mp4";
    mime_types[".doc"] = "application/msword";
    mime_types[".gz"] = "application/x-gzip";
    mime_types[".xml"] = "application/xml";
    mime_types[".pdf"] = "application/pdf";
    mime_types[".mp3"] = "audio/mpeg";
    mime_types[".wav"] = "audio/wav";
}

Mime &operator=(Mime const &copy)
{
    return (*this);
}

Mime::~Mime() {}

std::string Mime::getMimeType(std::string extn)
{
    if (mime_types.count(extn))
        return (mime_types[extn]);
    return (mime_type[default]);
}

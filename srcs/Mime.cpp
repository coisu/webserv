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

Mime::~Mime() {}

std::string Mime::getMimeType(std::string extn)
{
    if (mime_types.count(extn))
        return (mime_types[extn]);
    return (mime_type[default]);
}

// typ0es {
//   text/html                             html htm shtml;
//   text/css                              css;
//   text/xml                              xml rss;
//   image/gif                             gif;
//   image/jpeg                            jpeg jpg;
//   application/x-javascript              js;
//   text/plain                            txt;
//   text/x-component                      htc;
//   text/mathml                           mml;
//   image/png                             png;
//   image/x-icon                          ico;
//   image/x-jng                           jng;
//   image/vnd.wap.wbmp                    wbmp;
//   application/java-archive              jar war ear;
//   application/mac-binhex40              hqx;
//   application/pdf                       pdf;
//   application/x-cocoa                   cco;
//   application/x-java-archive-diff       jardiff;
//   application/x-java-jnlp-file          jnlp;
//   application/x-makeself                run;
//   application/x-perl                    pl pm;
//   application/x-pilot                   prc pdb;
//   application/x-rar-compressed          rar;
//   application/x-redhat-package-manager  rpm;
//   application/x-sea                     sea;
//   application/x-shockwave-flash         swf;
//   application/x-stuffit                 sit;
//   application/x-tcl                     tcl tk;
//   application/x-x509-ca-cert            der pem crt;
//   application/x-xpinstall               xpi;
//   application/zip                       zip;
//   application/octet-stream              deb;
//   application/octet-stream              bin exe dll;
//   application/octet-stream              dmg;
//   application/octet-stream              eot;
//   application/octet-stream              iso img;
//   application/octet-stream              msi msp msm;
//   audio/mpeg                            mp3;
//   audio/x-realaudio                     ra;
//   video/mpeg                            mpeg mpg;
//   video/quicktime                       mov;
//   video/x-flv                           flv;
//   video/x-msvideo                       avi;
//   video/x-ms-wmv                        wmv;
//   video/x-ms-asf                        asx asf;
//   video/x-mng                           mng;
// }
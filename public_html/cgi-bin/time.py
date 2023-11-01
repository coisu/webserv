#!/usr/bin/python3

import datetime
import cgi

head = "HTTP/1.1 200 OK\r\n" + "Content-type: text/html\r\n"
body = "<html>\r\n" + "<head>\r\n" + datetime.datetime.strftime(datetime.datetime.now(), "<h1>  %H:%M:%S </h1>") + "\r\n" + "</head>\r\n" + "</html>\r\n"
head += "Content-length: " + str(len(body)) + "\r\n\r\n"
print(head + body)
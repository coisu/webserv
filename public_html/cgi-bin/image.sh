#!/bin/bash

echo -e 'HTTP/1.1 200 OK'
echo -e 'Content-Type: image/jpeg\r\n'

# paste the full path to any image below
cat ./public_html/images/egs.jpg
echo -e '\r\n\r\n'
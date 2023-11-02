#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get filename here
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
   open(os.getcwd() + '/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded to ' + os.getcwd() + '/public_html/cgi-bin/'
else:
   message = 'Uploading Failed'

# Generate the HTML content
html_content = "<H1> " + message + " </H1>"

# Calculate Content-Length
content_length = len(html_content.encode('utf-8'))

# Print out the HTTP headers
print("HTTP/1.1 200 OK")
print("Content-Type: text/html;charset=utf-8")
print(f"Content-Length: {content_length}\r\n")

# Output the HTML content
print(html_content)


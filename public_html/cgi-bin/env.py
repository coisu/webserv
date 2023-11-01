#!/usr/bin/python3

import os

# Generate the HTML content first, so we can measure its length
html_content = []
html_content.append("<html>")
html_content.append("<head>")
html_content.append("<h2>Environment:</h2><br>")
html_content.append("</head>")
html_content.append("<body>")
for param in os.environ.keys():
    html_content.append("<b>%20s</b>: %s<br>" % (param, os.environ[param]))
html_content.append("</body>")
html_content.append("</html>")

# Convert the list of HTML lines into a single string
html_str = "\r\n".join(html_content)

# Calculate Content-Length
content_length = len(html_str)

# Print out the HTTP headers
print("HTTP/1.1 200 OK")
print(f"Content-Length: {content_length}")
print("Content-type: text/html\r\n\r\n")

# Output the HTML content
print(html_str)

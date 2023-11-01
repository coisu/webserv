#! /usr/bin/python3

import cgi
import sys
import os

form = cgi.FieldStorage()

arg1 = form.getvalue('f_num')
oper = form.getvalue('oper')
arg3 = form.getvalue('s_num')

response_body = []

if not oper or not arg1 or not arg3:
    response_body.append("Parameters are not correct.")
else:
    if len(oper) != 1:
        response_body.append("Parameters are not correct.")
    elif ord(oper) == 32:
        oper = '+'
    elif oper not in "+-/*^r":
        response_body.append("Parameters are not correct.")
    elif (arg1[0] in '-+' and not arg1[1:].isdigit()) or not arg1.isdigit():
        response_body.append("Parameters are not correct.")
    elif (arg3[0] in '-+' and not arg3[1:].isdigit()) or not arg3.isdigit():
        response_body.append("Parameters are not correct.")
    else:
        num1 = int(arg1)
        num2 = int(arg3)

        if num1 > 10000 or num2 > 10000:
            response_body.append("Enter a lower number.")
        else:
            result = 0

            if oper == '+':
                result = num1 + num2
            elif oper == '-':
                result = num1 - num2
            elif oper == '/' and num2 != 0:
                result = round(num1 / num2, 2)
            elif oper == '*':
                result = round(num1 * num2, 2)
            elif oper == '^':
                result = round(pow(num1, num2), 2)
            elif oper == 'r':
                if num2 != 0:
                    result = round(pow(num1, 1 / num2), 2)
                else:
                    result = "Cannot compute the root"
            else:
                result = "Operation not determined"

            response_body.append(f"<b>  = {result}</b> <br>")

# Convert the list of HTML lines into a single string
html_str = "\r\n".join(response_body)

# Calculate Content-Length
content_length = len(html_str.encode('utf-8'))

# Print out the HTTP headers
print("HTTP/1.1 200 OK")
print(f"Content-Length: {content_length}")
print("Content-type: text/html\r\n\r\n")

# Output the HTML content
print(html_str)


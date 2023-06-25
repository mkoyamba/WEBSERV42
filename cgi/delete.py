#!/usr/bin/python3
import sys
import os
import codecs

path = sys.argv[1]
Title = "SUCCESS"
Message = path + " successfully deleted!"

# Get the file from the form
if os.path.exists(path):
	os.remove(path)
	code = "200"
	code_message = " OK"
else:
	code = "204"
	code_message = " No content"

f = codecs.open(sys.argv[2])
body = f.read()
if code == 200:
	body = "<html>\n<body>\n<h1>" + Title + "</h1>\n<p>" + Message + "</p>\n<h3 style=\"color:black;\" >Come back to <a href=\"..\" style=\"color:black;\"> Homepage </a></h3>\n</body>\n</html>"
length = len(body)
print("HTTP/1.1 " + code + code_message)
print("Content-Type: text/html")
print("Content-Length: " + str(length) + "\r\n\r\n")
print(body)


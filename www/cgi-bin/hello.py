import cgitb
import cgi

cgitb.enable()

print("<!DOCTYPE html>")
print("<html>")
print("<body>")
form = cgi.FieldStorage()

print("<h1>Hello there!</h1>")
if "name" not in form or "addr" not in form:
    print("<h1>Error</h1>")
    print("<p>Please fill in the name and addr fields.</p>")
else:
    print("<p>name:" + form["name"].value + "</p>")
    print("<p>addr:" + form["addr"].value + "</p>")
print("</body>")
print("</html>")

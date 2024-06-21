t <html><head><title>Browser Language Preference</title></head>
i pg_header.inc
t <h2 align=center><br>Browser Language Preference</h2>
t <p><font size="2">You can use this information to create <b>multilingual</b>
t  websites.<br><br>
t  The language preferences can be set in Chrome via <b>Settings / Languages</b>
t  and in Mozilla Firefox via <b>Settings / General / Language</b>. You can change
t  the setting in your browser and reload this page to check.<br><br>
t  Your browser currently sends the following language setting:</font></p>
t <form action=index.htm method=post name=cgi>
t <input type=hidden value="lang" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_Server_CGI.C module
c e <tr><td><img src=pabb.gif>Browser Language</td><td><b>%s</b> [%s]</td>
t </tr></font></table><form>
i pg_footer.inc
. End of script must be closed with period.

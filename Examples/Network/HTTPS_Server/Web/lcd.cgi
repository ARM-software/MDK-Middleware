t <html><head><title>LCD Control</title></head>
i pg_header.inc
t <h2 align=center><br>LCD Module Control</h2>
t <p><font size="2">On this page you can change a message displayed on the LCD
t  module. Simply enter the text to be displayed and click on Send. The message
t  will be printed in a serial window in the debugger.<br><br>
t  This Form uses a <b>POST</b> method to send data to a Web server.</font></p>
t <form action=lcd.cgi method=post name=cgi>
t <input type=hidden value="lcd" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Line 1 Text</td>
c f1 <td><input type=text name=lcd1 size=20 maxlength=20 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Line 2 Text</TD>
c f2 <td><input type=text name=lcd2 size=20 maxlength=20 value="%s"></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=submit name=set value="Send" id="sbm">
t <input type=reset value="Undo">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.

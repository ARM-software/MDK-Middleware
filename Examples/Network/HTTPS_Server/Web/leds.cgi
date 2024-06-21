t <html><head><title>LED Control</title>
t <script language=JavaScript>
t function AllSW(st) {
t  for(i=0;i<document.form1.length;i++) {
t   if(document.form1.elements[i].type=="checkbox"){
t    document.form1.elements[i].checked=st;
t   }
t  }
t  document.form1.submit();
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Control LEDs on the board</h2>
t <p><font size="2">This page shows you how to use the following http form input
t  objects: checkbox, select and button. It also uses a simple java script function
t  to check/uncheck all checkboxes and submit the data.<br><br>
t  This form uses a <b>POST</b> method to send data to a web server.</font></p>
t <form action=leds.cgi method=post name=form1>
t <input type=hidden value="led" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
t <td><img src=pabb.gif>LED control:</td>
t <td><select name="ctrl" onchange="submit();">
c bc <option %s>Browser</option><option %s>Running Lights</option></select></td></tr>
t <tr><td><img src=pabb.gif>LED diode ports [7..0]:</td>
t <td><table><tr valign="middle">
# Here begin the 'checkbox' definitions
c b7 <td><input type=checkbox name=led7 OnClick="submit();" %s>7</td>
c b6 <td><input type=checkbox name=led6 OnClick="submit();" %s>6</td>
c b5 <td><input type=checkbox name=led5 OnClick="submit();" %s>5</td>
c b4 <td><input type=checkbox name=led4 OnClick="submit();" %s>4</td>
t <td width="5%"></td>
c b3 <td><input type=checkbox name=led3 OnClick="submit();" %s>3</td>
c b2 <td><input type=checkbox name=led2 OnClick="submit();" %s>2</td>
c b1 <td><input type=checkbox name=led1 OnClick="submit();" %s>1</td>
c b0 <td><input type=checkbox name=led0 OnClick="submit();" %s>0</td>
t </font></table></td></tr>
t <tr><td><img src=pabb.gif>All LED diodes On or OFF</td>
t <td><input type=button value="&nbsp;&nbsp;ON&nbsp;&nbsp;&nbsp;" onclick="AllSW(true)">
t <input type=button value="&nbsp;&nbsp;OFF&nbsp;&nbsp;" onclick="AllSW(false)"></td></tr>
t </table></form>
i pg_footer.inc
. End of script must be closed with period.

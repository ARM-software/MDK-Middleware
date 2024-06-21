t <html><head><title>Pushbuttons</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("buttons.cgx", 300);
t function periodicUpdate() {
t  if(document.getElementById("refreshChkBox").checked == true) {
t   updateMultiple(formUpdate);
t   periodicFormTime = setTimeout("periodicUpdate()", formUpdate.period); 
t  }
t  else
t   clearTimeout(periodicFormTime);
t }
t </script></head>
i pg_header.inc
t <h3 align="center"><br>Digital input</h3>
t <p><font size="2">On this page you can monitor the status of the pushbuttons
t  on board. The periodic refresh of the screen is based on <b>xml</b> technology.
t  This results in a smooth, flicker-free screen refresh.<br><br>
t  Press a button on an evaluation board and observe the change on the screen.</font></p>
t <form action="buttons.cgi" method="post" id="form1" name="form1">
t <table border="0" width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Status</th>
t </tr>
t <tr>
t  <td><img src="pabb.gif">Buttons [7..0]:</td>
t  <td align="center">
t   <input type="checkbox" disabled id="button7">7
t   <input type="checkbox" disabled id="button6">6
t   <input type="checkbox" disabled id="button5">5
t   <input type="checkbox" disabled id="button4">4
t   <input type="checkbox" disabled id="button3">3
t   <input type="checkbox" disabled id="button2">2
t   <input type="checkbox" disabled id="button1">1
t   <input type="checkbox" disabled id="button0">0
t  </td>
t </tr>
t </font></table>
t <p align="center">
t  <input type="button" id="refreshBtn" value="Refresh" onclick="updateMultiple(formUpdate)">
t  Periodic:<input type="checkbox" id="refreshChkBox" onclick="periodicUpdate()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.

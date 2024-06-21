t <html><head><title>Network Settings</title>
t <script language=JavaScript>
t function changeConfirm(f){
t  if(!confirm('Are you sure you want to change\nthe Network settings?')) return;
t  f.submit();
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Network Settings</h2>
t <p><font size="2">Here you can change the network settings of the system.
t  After you have changed the IP address, you must also change the host IP address
t  in your Internet browser to re-establish the connection to the target.<br><br>
t  This form uses a <b>GET</b> method to send data to a web server.</font></p>
t <form action=network.cgi method=get name=cgi>
t <input type=hidden value="net" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=20%>IPv4</th>
t  <th width=40%>IPv6</th>
t </tr>
# Here begin data setting which is formatted in HTTP_Server_CGI.c module
t <tr>
t    <td><img src=pabb.gif>IP Address</td>
c ai <td><input type=text name=ip value="%s" size=15 maxlength=15></td>
c aj <td><input type=text name=ip6 value="%s" size=40 maxlength=40></td>
t </tr>
t <tr>
t    <td><img src=pabb.gif>Netmask/Link-local Address</td>
c am <td><input type=text name=msk value="%s" size=15 maxlength=15></td>
c al <td><input type=text name=lla value="%s" size=40 maxlength=40 disabled></td>
t </tr>
t <tr>
t    <td><img src=pabb.gif>Default Gateway</td>
c ag <td><input type=text name=gw value="%s" size=15 maxlength=15></td>
c ah <td><input type=text name=gw6 value="%s" size=40 maxlength=40></td>
t </tr>
t <tr>
t    <td><img src=pabb.gif>Primary DNS Server</td>
c ap <td><input type=text name=pdns value="%s" size=15 maxlength=15></td>
c aq <td><input type=text name=pdns6 value="%s" size=40 maxlength=40></td>
t </tr>
t <tr>
t    <td><img src=pabb.gif>Secondary DNS Server</td>
c as <td><input type=text name=sdns value="%s" size=15 maxlength=15></td>
c at <td><input type=text name=sdns6 value="%s" size=40 maxlength=40></td>
t </tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=button name=set value="Change" onclick="changeConfirm(this.form)">
t <input type=reset value="Undo">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.


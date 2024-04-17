t <html><head><title>Format SD Card</title>
t <script language=JavaScript>
t  function formatConfirm(f){
t   if(!confirm('Are you sure you want to format SD Card?')) return;
t   f.submit();
t  }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Format SD Card</h2>
t <p><font size="3"><br>You must format the SD memory card before using it for
t  the first time or if the FAT file system is damaged.<br><br>
t  It is strongly recommended that you format the SD memory card with this utility
t  before using it for the first time. This will optimize the cluster size and
t  cluster alignment for the best SD card performance.</font></p>
t <form action=index.htm method=post name=form1>
t <p><font size="3"><br>SD Card Volume Label:
c f1 <input type=text name=label size=11 maxlength=11 value="%s">
t </font></p>
t <input type=hidden value="yes" name=format>
t <p align="center">
t  <input type=button value="Format SD Card" onclick="formatConfirm(this.form)">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.

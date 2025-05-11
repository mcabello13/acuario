t <html>
t <head><title>Alimentacion y Calidad del Agua</title>
t     <meta charset="utf-8">
t     <meta name="viewport" content="width=device-width, initial-scale=1">
t     <title>Alimentacion y Calidad del Agua</title>
t     <link href="style.css" rel="stylesheet">
t </head>
t
t <body>
t <header class="header">
t   <div class="topnav">
t     <a href="/index.htm">Home</a>
t   </div>
t </header>
t <br><br>
t <head>
t <script language="javascript">
t     </script>
t <script
t     type="text/javascript"
t      src="https://www.gstatic.com/charts/loader.js"></script>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("ad.cgx", 500);
t function plotADGraph() {
t adVal = document.getElementById("ad_value").value;
t numVal = parseInt(adVal, 16);
t voltsVal = (1000*3.3*numVal)/4096;
t tableSize = (numVal*100/3);
t document.getElementById("ad_table").style.width = (tableSize + '%');
t document.getElementById("ad_volts").value = (voltsVal.toFixed(3) + ' V');
t }
t function periodicUpdateAd() {
t if(document.getElementById("adChkBox").checked == true) {
t updateMultiple(formUpdate,plotADGraph);
t ad_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t }
t else
t clearTimeout(ad_elTime);
t }
t function limpieza() {
t }
t </script>
t <h2 align="center"><br>Alimentacion y Calidad del Agua</h2>
t <form action="ad.cgi" method="post" name="ad">
t <input type="hidden" value="ad" name="pg">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: transparent">
t  <th width=15%></th>
t  <th width=15%></th>
t  <th width=15%></th>
t  <th width=55%>Turbidez del Agua</th></tr>
t <tr><td><img src="pabb.gif">OPCIONES DISPONIBLES:</td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c g 1  size="10" id="ad_value" value="0x%03X"></td>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c g 2  size="10" id="ad_volts" value="%5.3f V"></td>
t <td height=50><table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%"><tr>
c g 3 <td><table id="ad_table" style="width: %d%%" border="0" cellpadding="0" cellspacing="0">
t <tr><td bgcolor="#FF0000">&nbsp;</td></tr></table></td></tr></table></td></tr>
# Here begin the 'checkbox' definitions
c b 8 <td><input type=checkbox name=activarLimpieza OnClick="submit();"> Limpiar</td></tr>
c b 9 <td><input type=checkbox name=activarAlimentacion OnClick="submit();"> Alimentacion</td></tr>
c b 10 <td><input type=checkbox name=activarBomba OnClick="submit();"> Activar Bomba de Agua</td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotADGraph)">
t Medida Periodica:<input type="checkbox" id="adChkBox" onclick="periodicUpdateAd()">
t <p><font size="5">ATENCION: 
t En la barra deslizante puede observarse el nivel de turbidez del agua del acuario,  
t si el nivel alcanzado es demasiado alto, se recomienda activar el modo limpieza pulsando
t en el checkbox Limpiar. 
t Si desea alimentar a los peces, pulse el checkbox Alimentacion para abrir la trampilla. 
t Si desea activar la Bomba de Agua, pulse el checkbox Activar Bomba de Agua.</font></p>
t </p></form>
. End of script must be closed with period

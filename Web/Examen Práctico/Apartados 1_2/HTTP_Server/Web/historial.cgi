t <html>
t <head><title>Historial de Medidas del Acuario</title>
t     <meta charset="utf-8">
t     <meta name="viewport" content="width=device-width, initial-scale=1">
t     <link href="style.css" rel="stylesheet">
t </head>
t <body>
t <header class="header">
t   <div class="topnav">
t     <a href="/index.htm">Home</a>
t   </div>
t </header>
t <br><br>
t <script type="text/javascript" src="xml_http.js"></script>
t <script type="text/javascript">
t   var formHistorial = new periodicObj("historial.cgx", 1000);
t   function periodicUpdate() {
t     updateMultiple(formHistorial);
t   }
t   updateMultiple(formHistorial);
t   setInterval(periodicUpdate, 2000);
t </script>
t <style>
t table, th, td {
t   border: 1px solid white;
t   border-collapse: collapse;
t   padding: 6px;
t   text-align: center;
t   color: white;
t }
t th {
t   background-color: #5A7A88;
t }
t body {
t   font-family: Arial, sans-serif;
t   background-color: black;
t   color: white;
t   text-align: center;
t }
t </style>
t <h2>Historial de Medidas del Acuario</h2>
# --- BOTÓN DE BORRADO ---
t <form method="POST" action="historial.cgi" style="margin-bottom: 20px;">
t   <input type="submit" name="borrarMemoria" value="Borrar Historial"
t     style="padding: 10px 20px; font-weight: bold; font-size: 16px;
t     background-color: #B22222; color: white; border: none;
t     border-radius: 5px; cursor: pointer;">
t </form>
# --- TABLA DE HISTORIAL ---
t <form>
t <table style="width:95%; margin:auto;">
t <tr>
t <th>#</th>
t <th>Fecha y Hora</th>
t <th>Luz</th>
t <th>pH</th>
t <th>Turbidez</th>
t <th>Temperatura(ºC)</th>
t <th>Corriente(A)</th>
t </tr>
t <tbody id="historial">
c h 0 %s
t </tbody>
t </table>
t </form>
t </body>
t </html>
.

t <html>
t <head><title>Consumo</title>
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
t <head>
t <script
t     type="text/javascript"
t     src="https://www.gstatic.com/charts/loader.js"></script>
t <script language="JavaScript" type="text/javascript" src="xml_http.js"></script>
t <script language="JavaScript" type="text/javascript">
t var formUpdate = new periodicObj("consumo.cgx", 500);
t var corrienteVal;
t var timeutcVal;
t function periodicUpdate() {
t   updateMultiple(formUpdate);
t   corrienteVal = parseFloat(document.getElementById("corriente").value);
t   timeutcVal = document.getElementById("timeutc").value;
t }
t updateMultiple(formUpdate);
t var id = setInterval('periodicUpdate();', 1500);
t google.charts.load('current', { packages: ['corechart', 'line'] });
t google.charts.setOnLoadCallback(drawCorrienteChart);
t function drawCorrienteChart() {
t   let data = google.visualization.arrayToDataTable([
t     ['Tiempo', 'Corriente (A)'],
t     [0, 0]
t   ]);
t   let options = {
t     title: 'Consumo del Acuario (Corriente)',
t     legend: { textStyle: { color: '#FFF' } },
t     titleTextStyle: { color: '#FFF' },
t     hAxis: { textStyle: { color: '#FFF' }, title: 'Tiempo' },
t     vAxis: { title: 'Corriente (A)', titleTextStyle: { color: '#FFF' }, textStyle: { color: '#FFF' } },
t     colors: ['#FF6B33'],
t     backgroundColor: { fill: 'transparent' }
t   };
t   let chart = new google.visualization.LineChart(document.getElementById('chart_div'));
t   chart.draw(data, options);
t   let maxDatas = 50;
t   let index = 1;
t   setInterval(function () {
t     if (!isNaN(corrienteVal)) {
t       if (data.getNumberOfRows() > maxDatas) {
t         data.removeRows(0, data.getNumberOfRows() - maxDatas);
t       }
t       data.addRow([index, corrienteVal]);
t       chart.draw(data, options);
t       index++;
t     }
t   }, 1000);
t }
t </script>
t </head>
t <style>
t table tr {
t   border: 1px solid white;
t   background-color: #5A7A88;
t   color: white;
t }
t </style>
t <div id="wrapper">
t   <div id="chart_div" style="width: 100%; height: 500px;"></div>
t </div>
t <table style="width:100%">
t   <tr>
t     <th>Corriente</th>
t     <th>Hora</th>  
t     <th>Fecha</th>  
t   </tr>
t   <tr>
t     <td><input type="float" readonly size="10" id="corriente" value="0.00"></td>
t     <td><input type="text" readonly size="10" id="timeutc" value="--:--:--"></td>
t     <td><input type="text" readonly size="10" id="dateutc" value="--/--/----"></td>
t   </tr>
t </table>
t </body>
t </html>
. End of script must be closed with period.

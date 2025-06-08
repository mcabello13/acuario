t <html>
t <head><title>Estadisticas</title>
t     <meta charset="utf-8">
t     <meta name="viewport" content="width=device-width, initial-scale=1">
t     <title>Estadisticas del Acuario</title>
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
t var formUpdate = new periodicObj("buttons.cgx", 500);
t function plotLuz() {
t   const alertBox = document.getElementById("alertBox");
t   luzVal = document.getElementById("luz");
t   
t   if (document.getElementById("luzVal").value <= 1.5) {
t     alertBox.style.display = "block";
t   } else {
t     alertBox.style.display = "none";
t   }
t }
t function plotPh() {
t  phVal = document.getElementById("ph").value;
t }
t function plotTemperatura() {
t  temperaturaVal = document.getElementById("temperatura").value;
t }
t function plotTimeUTC() {
t  dateVal = document.getElementById("timeutc").value;
t }
t var luzVal;
t var phVal;
t var temperaturaVal;
t function periodicUpdate() {
t updateMultiple(formUpdate);
t luzVal=document.getElementById("luz").value;
t updateMultiple(formUpdate);
t phVal=document.getElementById("ph").value;
t updateMultiple(formUpdate);
t temperaturaVal=document.getElementById("temperatura").value;
t updateMultiple(formUpdate);
t timeutcVal=document.getElementById("timeutc").value;
t }
t updateMultiple(formUpdate);
t var id = setInterval('periodicUpdate();',1500);
t      // load current chart package
t      google.charts.load('current', {
t        packages: ['corechart', 'line'],
t      });
t
t     // set callback function when api loaded
t  google.charts.setOnLoadCallback(drawChart2);
t  google.charts.setOnLoadCallback(drawChart3);
t  google.charts.setOnLoadCallback(drawChart4);
t
t function drawChart2() {
t    // create data object with default value
t    let data = google.visualization.arrayToDataTable([
t    ['Time','lux'],
t    [0,0],
t    ]);
t
t  // create options object with titles, colors, etc.
t  let options = {
t    title: 'Luminosidad del Acuario',
t     legendTextStyle: { color: '#FFF' },
t     titleTextStyle: { color: '#FFF' },
t    hAxis: {
t     textPosition: 'none',
t    },
t    vAxis: {
t     title: 'lux(%)',
t       titleTextStyle:{color: '#FFF'},
t       textStyle:{color: '#FFF'}
t    },
t    backgroundColor: { fill:'transparent' }
t   };
t
t   // draw chart on load
t  let chart = new google.visualization.LineChart(
t    document.getElementById('chart2_div')
t  );
t  chart.draw(data, options);
t
t  // max amount of data rows that should be displayed
t  let maxDatas = 50;
t
t       // interval for adding new data every 250ms
t        let index = 0;
t        setInterval(function () {
t          // instead of this random, you can make an ajax call for the current cpu usage or what ever data you want to display
t          let randomRAM = parseFloat(luzVal);
t
t         if (data.getNumberOfRows() > maxDatas) {
t            data.removeRows(0, data.getNumberOfRows() - maxDatas);
t          }
t
t         data.addRow([index, randomRAM]);
t          chart.draw(data, options);
t
t         index++;
t        }, 1000);
t      }
t function drawChart3() {
t    // create data object with default value
t    let data = google.visualization.arrayToDataTable([
t    ['Time','pH'],
t    [0,0],
t    ]);
t
t  // create options object with titles, colors, etc.
t  let options = {
t    title: 'Nivel de pH del Acuario',
t     legendTextStyle: { color: '#FFF' },
t     titleTextStyle: { color: '#FFF' },
t    hAxis: {
t     textPosition: 'none',
t    },
t    vAxis: {
t     title: 'pH',
t       titleTextStyle:{color: '#FFF'},
t       textStyle:{color: '#FFF'}
t    },
t 	 colors: ['#FF6B33'],
t    backgroundColor: { fill:'transparent' }
t   };
t
t   // draw chart on load
t  let chart = new google.visualization.LineChart(
t    document.getElementById('chart3_div')
t  );
t  chart.draw(data, options);
t
t  // max amount of data rows that should be displayed
t  let maxDatas = 50;
t
t       // interval for adding new data every 250ms
t        let index = 0;
t        setInterval(function () {
t          // instead of this random, you can make an ajax call for the current cpu usage or what ever data you want to display
t          let randomRAM = parseFloat(phVal);
t
t         if (data.getNumberOfRows() > maxDatas) {
t            data.removeRows(0, data.getNumberOfRows() - maxDatas);
t          }
t
t         data.addRow([index, randomRAM]);
t          chart.draw(data, options);
t
t         index++;
t        }, 1000);
t      }
t function drawChart4() {
t    // create data object with default value
t    let data = google.visualization.arrayToDataTable([
t    ['Time','ºC'],
t    [0,0],
t    ]);
t
t  // create options object with titles, colors, etc.
t  let options = {
t    title: 'Temperatura del Acuario',
t     legendTextStyle: { color: '#FFF' },
t     titleTextStyle: { color: '#FFF' },
t    hAxis: {
t     textPosition: 'none',
t    },
t    vAxis: {
t     title: 'ºC',
t       titleTextStyle:{color: '#FFF'},
t       textStyle:{color: '#FFF'}
t    },
t 	 colors: ['#4cff33'],
t    backgroundColor: { fill:'transparent' }
t   };
t
t   // draw chart on load
t  let chart = new google.visualization.LineChart(
t    document.getElementById('chart4_div')
t  );
t  chart.draw(data, options);
t
t  // max amount of data rows that should be displayed
t  let maxDatas = 50;
t
t       // interval for adding new data every 250ms
t        let index = 0;
t        setInterval(function () {
t          // instead of this random, you can make an ajax call for the current cpu usage or what ever data you want to display
t          let randomRAM = parseFloat(temperaturaVal);
t
t         if (data.getNumberOfRows() > maxDatas) {
t            data.removeRows(0, data.getNumberOfRows() - maxDatas);
t          }
t
t         data.addRow([index, randomRAM]);
t          chart.draw(data, options);
t
t         index++;
t        }, 1000);
t      }
t    </script>
t   </head>
t <style>
t table tr {
t  border:1px solid white;
t  background-color:#5A7A88;
t  color: white;
t }
t </style>
t <div id="wrapper">
t  <div id="chart_div"></div>
t  <div id="chart2_div"></div>
t  <div id="chart3_div"></div>
t  <div id="chart4_div"></div>
t  </div>
t <table style="width:100%">
t  <tr>
t    <th>Luminosidad</th>
t    <th>pH</th>
t    <th>Temperatura</th>
t    <th>Hora</th>  
t    <th>Fecha</th>  
t  </tr>
t  <tr>
t <td><input type="float" readonly
c m	1 size="10" id="luz" value="%f" ></td>
t <td><input type="float" readonly
c m	2 size="10" id="ph" value="%f" ></td>
t <td><input type="string" readonly
c m	3 size="10" id="temperatura" value="%f" ></td>
t <td><input type="int" readonly
c m	4 size="10" id="timeutc" value="%d" ></td>
t <td><input type="int" readonly
c m	5 size="10" id="dateutc" value="%d" ></td>
t  </tr>
t </table>
t <div id="alertBox" style="display: none;" class="custom-alert">
t  <strong>Atencion</strong> , se ha entrado en Modo Bajo Consumo.<br>
t  </span> 
t </div>
t </body>
t </html>
. End of script must be closed with period.

var array_length = 10;
var xValues = [];

for (let i = 0; i < array_length; i++) {
  xValues[i] = i;
}

var yValues = [];


const myChart1 = new Chart("myChart1", {
  type: "line",  
  data: {
    labels: xTempValues,
    datasets: [{
      fill: false,
      lineTension: 0,
      backgroundColor: "rgba(0,0,255,1.0)",
      borderColor: "rgba(0,0,255,0.1)",
      data: yTempValues
    }]
  },
  options: {
    title:{
         display: true,
        text:"Temperature Measurment(C)"
  },
    legend: {display: false},
    scales: {
      yAxes: [{
         scaleLabel:
          {
            display: true,
            labelString:"Temperature(C)"
          },
        ticks: {min: 0, max: 40}

      }],
      xAxes: [{
        display: false
      },
      {
        scaleLabel:
        {
          display: true,
          labelString: "Measurement (Number)"
        }
      }]
    }
  }
});

const myChart2 = new Chart('myChart2', {
  type: "line",  
  data: {
    labels: xTemp2Values,
    datasets: [{
      fill: false,
      lineTension: 0,
      backgroundColor: "rgba(0,0,255,1.0)",
      borderColor: "rgba(0,0,255,0.1)",
      data: yTemp2Values
    }],      
  },
  options: { 
    title:{
         display: true,
        text:"Temperature Measurment(F)"
  },
    legend: { display: false},
    scales: {
      yAxes: [{
        scaleLabel:
        {
          display: true,
          labelString: "Temperature (F)"
        },
        ticks: { min: 0, max: 100 }
      }],
      xAxes: [{
        display: false
      },
      {
        scaleLabel:
        {
          display: true,
          labelString: "Measurement (Number)"
        }
      }]
    }
  }
});


const myChart3 = new Chart('myChart3', {
  type: "line",  
  data: {
    labels: xHumValues,
    datasets: [{
      fill: false,
      lineTension: 0,
      backgroundColor: "rgba(0,0,255,1.0)",
      borderColor: "rgba(0,0,255,0.1)",
      data: yHumValues
    }],      
  },
  options: { 
    title:{
         display: true,
        text:"Humidity Measurment"
  },
    legend: { display: false},
    scales: {
      yAxes: [{
        scaleLabel:
        {
          display: true,
          labelString: "Humidity(%)"
        },
        ticks: { min: 0, max: 100 }
      }],
      xAxes: [{
        display: false
      },
      {
        scaleLabel:
        {
          display: true,
          labelString: "Measurement(Number)"
        }
      }]
    }
  }
});




var slider = document.getElementById('ID_RANDOM_INTENSITY');
var output = document.getElementById('ID_RANDOM_INTENSITY_VALUE');
slider.addEventListener('change', slider_changed);

var Socket;

function init() {
  Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  Socket.onmessage = function(event) {
    processCommand(event);
  };
}

function slider_changed () {
  var l_random_intensity = slider.value;
  console.log(l_random_intensity);
  var msg = { type: "random_intensity", value: l_random_intensity};
  Socket.send(JSON.stringify(msg)); 
}

function processCommand(event) {
  var obj = JSON.parse(event.data);
  var type = obj.type;
  if (type.localeCompare("random_intensity") == 0) { 
    var l_random_intensity = parseInt(obj.value); 
    console.log(l_random_intensity); 
    slider.value = l_random_intensity; 
    output.innerHTML = l_random_intensity;
  }
  else if (type.localeCompare("graph_update") == 0) {
    console.log(obj.value);
    myChart.data.datasets[0].data = obj.value;
    myChart.update();
  }
}
window.onload = function(event) {
  init();
}
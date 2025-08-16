
var array_length = 10;

var xTempValues  = [];
var xTemp2Values = []; 
var xHumValues   = [];



for (let i = 0; i < array_length; i++) 
{
  xTempValues[i]  = i;
  xTemp2Values[i] = i;
  xHumValues[i]   = i;
}

var Socket; 
var green_statue  = "off";
var red_statue    = "off";
var yellow_statue = "off";

var yTemp2Values = [];
var yTempValues  = [];
var yHumValues   = [];

let green_led_button = document.getElementById('Green_Led_Control');
    green_led_button.addEventListener('click', green_led_change);
let red_led_button = document.getElementById('Red_Led_Control');
    red_led_button.addEventListener('click', red_led_change);
let  yellow_led_button = document.getElementById('Yellow_Led_Control');
     yellow_led_button.addEventListener('click', yellow_led_change);
let mytable = document.getElementById('sensor_reading');

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
        ticks: {min: 20, max: 30}

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
        ticks: { min: 70, max: 80 }
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
        ticks: { min: 50, max: 75 }
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

function green_led_change(event) 
{ 
   
  if(green_statue == "off")
  {  
    event.target.style.backgroundColor = "green";
    green_led_button.innerHTML = "Green_Led_On";   
    green_statue = "on";	
	var info = {action:1}; 
    Socket.send(JSON.stringify(info));
  }
  else
  {     
	var info = {action: 2}; 
	event.target.style.backgroundColor = "grey";
    green_led_button.innerHTML = "Green_Led_Off"; 
    green_statue = "off";	
    Socket.send(JSON.stringify(info));	  
  }  
}

function yellow_led_change(event) 
{ 
   
  if(yellow_statue == "off")
  {  
    event.target.style.backgroundColor = "yellow";
    yellow_led_button.innerHTML = "Yellow_Led_On";   
    yellow_statue = "on";	
	var info = {action:5}; 
    Socket.send(JSON.stringify(info));
  }
  else
  {     
	var info = {action: 6}; 
	event.target.style.backgroundColor = "grey";
    yellow_led_button.innerHTML = "Yellow_Led_Off"; 
    yellow_statue = "off";	
    Socket.send(JSON.stringify(info));	  
  }  
}



function red_led_change(event) 
{   
     
  if(red_statue == "off")
  {  
    event.target.style.backgroundColor = "red";
    red_led_button.innerHTML = "Red_Led_On";   
    red_statue = "on";	
	var info = {action:3}; 
        Socket.send(JSON.stringify(info));
  }
  else
  {  
    red_statue = "off";
	var info = {action: 4}; 
	event.target.style.backgroundColor = "grey";
    red_led_button.innerHTML = "Red_Led_Off";    
    Socket.send(JSON.stringify(info));	  
  }
  
}



function init() 
{ 
	Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); 
	Socket.onmessage = function(event) { processCommand(event); }; 
} 



function processCommand(event) {
  var obj = JSON.parse(event.data);
  var type = obj.type;
  var updatedValue = obj.updatedVal; 
  if (type.localeCompare("temperature_update_c") == 0) {
    console.log(obj.value);
    console.log(updatedValue);
    myChart1.data.datasets[0].data = obj.value;
    myChart1.update();
	  mytable.rows[1].cells[0].innerText = updatedValue;	
  }
  else if (type.localeCompare("temperature_update_f") == 0) {
    console.log(obj.value);
    myChart2.data.datasets[0].data = obj.value;
    myChart2.update();
	  mytable.rows[1].cells[1].innerText = updatedValue;
  }
 else if (type.localeCompare("humidity_update") == 0)  
  {
    console.log(obj.value);
    myChart3.data.datasets[0].data = obj.value;
    myChart3.update();
	  mytable.rows[1].cells[2].innerText = updatedValue;
  }
}


window.onload = function(event) {
  init();
}
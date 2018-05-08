//Author: Jaydeep Rotithor
//Description: Client file that creates a socket connection to the web map page and sends data through it in real time

var express = require('express');
var app = express();
var $ = jQuery = require('jquery');
require('./js/jquery-csv.js');
var http = require("http");
var server = http.createServer(app);
var io = require('socket.io')(server);
var fs = require('fs');
var spawn = require("child_process").spawn;
var p = spawn("python", ["py2csv.py"], {detached: true});
var valid_data;



//Listens on port 5040 to avoid collisions with existing processes
server.listen(process.env.PORT || 5040)

app.use(express.static(__dirname ));



	//creates the socket connection and sends the data from the csv file through it
    io.sockets.on('connection', function (socket) {

			
			
			var data = fs.readFileSync('sample_data.csv', 'utf8');
			var data_subsets = $.csv.toObjects(data);
			if(!(data_subsets.length == 0))
			{
				valid_data = data_subsets
			}
            socket.emit('join',  valid_data);



    });





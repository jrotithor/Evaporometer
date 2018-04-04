var express = require('express');
var app = express();
var $ = jQuery = require('jquery');
require('./js/jquery-csv.js');
var http = require("http");
var server = http.createServer(app);
var io = require('socket.io')(server);
var fs = require('fs');






server.listen(80)

app.use(express.static(__dirname ));




    io.sockets.on('connection', function (socket) {

			

			var data = fs.readFileSync('sample_data.csv', 'utf8');
			var data_subsets = $.csv.toObjects(data);
            socket.emit('join',  data_subsets);



    });





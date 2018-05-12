# PHAZE


This repository contains all of the code used to create the web map and charts that are part of the geovisualization project for the PHAZE Weather Station.

The following files contain code used to create the geovisualization project:

####index.html

Contains the actual web map. It contains the HTML and CSS used to render the features on the web page, and it also contains javascript that:

Contains a socket.io client node to receive real-time data from app.js,

Updates the features displayed on the map if the user clicks a button, 

Places the data into the appropriate charts and maps based on its category(temperature, humidity, loadcell, or battery voltage), and

Uses AJAX to automatically refresh the webpage every 5 minutes to plot new data



#### app.js 

Server file that:

Creates a socket.io server node that opens up the csv file containing live data, parses it into a JSON object, and sends it through the socket connection.



#### py2csv.py

Uses Google API to pull data from the Google Spreadsheet and convert it to a csv. 
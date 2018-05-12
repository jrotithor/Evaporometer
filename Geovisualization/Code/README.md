# PHAZE


This repository contains all of the code used to create the web map and charts that are part of the geovisualization project for the PHAZE Weather Station.

The following files contain code used to create the geovisualization project:

#### index.html

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



### Using the Web Map

If you want to access the web map, go to the following URL:

###### https://pacific-castle-38978.herokuapp.com/

The web map will slow down significantly 20 seconds after it is launched. This is because it is running a refresh to gather the latest data. This will last for about 5 seconds. After this, it will automatically refresh once every 5 minutes to gather the data.

If nothing is showing up on the web map, that means that Heroku has made the app idle (due to long periods of inactivity), and by launching the app, Heroku is attempting to gather the data, but has none stored. However, since it takes a few seconds to fully gather the data, please wait 20 seconds until the refresh is run. The maps and charts will load after that.

#### Features

There are buttons above the web map that, when clicked by the user, will change what is displayed on the map. The legend in the top right will also change to match the corresponding measurement. The color of the markers on the map will change to match the data.

If the markers on the map are clicked on, a popup box will appear, and this will show the measurement and the most recent data collected for it.

The map is interactive, so the user can scroll and zoom around on it.

The charts display each data point gathered for each device. There are 2 charts for each measurement, one for each device. There are some gaps in the data, and these gaps indicate that the Evaporometers were not collecting any data during these time periods. The X-axis may become crowded as the number of measurements increases, but if the user hovers their mouse over a data point, a popup box will appear that indicates the date and time the data point was collected, the measurement that the data is for, and the data collected.


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

If nothing is showing up on the web map, that means that Heroku has made the app idle (due to long periods of inactivity), and by launching the app, Heroku is attempting to gather the data, but has none stored. It takes approximately 30-40 seconds for the app to gather all of the data, so if the web page is launched during this period, it may result in an error. This is because when Heroku idles the app, it shuts down all associated processes and deletes the file in which data is collected to be sent to the web page. Because this file takes 30-40 seconds to generate, the user should briefly launch the page, close it, and then open it again when the data file is generated. Once it is generated, the app will run smoothly until Heroku terminates it again, which occurs after the web map is unused for longer than 30 minutes.

#### Features

The following is a brief rundown of all of the features available to the user of the web map.

###### Buttons

There are buttons above the web map that, when clicked by the user, will change what is displayed on the map. The legend in the top right will also change to match the corresponding measurement. The color of the markers on the map will change to match the data.

###### Spin Boxes

Above these buttons, there are 4 boxes. The top 2 boxes allow the user to select the start and end dates for the data displayed on the charts for device 1. The bottom 2 boxes allow the user to select the start and end dates for the data displayed on the charts for device 2. 

The spin boxes ensure that the dates selected by the user are valid. If the user tries to select a start date that is later than the end date, the start date will be reset to match the end date, and only the data point at that specific time will be displayed for that device. If the user tries to select a start date that is earlier than the first data point, the spin box will be reset to the first data point. If the user tries to select an end date that is later than the most recent data point, that spin box will be reset to the most recently collected data point.

There are gaps in the data, and as such, it is possible for the user to select a date which is between the start date and the end date, but does not have any data. If this is done, the date will be rounded down to the nearest earlier data point. For example, if the user selects a start date of April 17, 2018 at 3:19 PM and an end date of April 17, 2018 at 3:26 PM, the end date would be rounded down to April 17, 2018 at 3:24 PM, since that would be the nearest earlier data point(the devices collect data once every 5 minutes).

If the user makes the range of data points very large, the webpage will become very slow, since there would be thousands of data points coming in at once and they all need to be put on the charts.

###### Interactive Map Markers

If the markers on the map are clicked on, a popup box will appear, and this will show the measurement and the most recent data collected for it.

The map is interactive, so the user can scroll and zoom around on it.

###### Chart Features

The charts display each data point gathered for each device. There are 2 charts for each measurement, one for each device. There are some gaps in the data, and these gaps indicate that the Evaporometers were not collecting any data during these time periods. The X-axis may become crowded as the number of measurements increases, but if the user hovers their mouse over a data point, a popup box will appear that indicates the date and time the data point was collected, the measurement that the data is for, and the data collected.

#### Development Guide

The following sections describe how future developers can add to the map.

##### What You Need Installed

In order to be able to work on the web map, you will need to have the following installed: A recent version of python, node.js, npm, and gspread. All of the necessary javascript and css dependencies are in their respective folders.

##### Adding More Devices

Currently, the web map is set up to display data from only two devices, since that is how many are deployed. In the future, it would be good to add more devices to get a better idea of weather patterns at deployment sites. The information below will guide the user on how to add devices. There is a lot of information, but it is in the same order as the code, so you can choose which section to look at based on your need.

###### Data file

First, you will need access to the Google Spreadsheet on which data is being collected. Please contact the OpenS lab to gain this access.

###### Creating Google API Credentials

You will need to make sure that Python is installed on your computer. The latest version (Python 3.6) is ideal. You will then need to download gspread. Please follow the instructions on the following website:

https://github.com/burnash/gspread

At the top of the instruction set, there is a link to installing OAuth2 credentials. You will need to follow these instructions to create a Google Drive API. The reason that you need to do this is that you need to download a credentials file to "log in" to your Google Drive API. This may seem rather cumbersome, but it is far more secure than entering your Google username and password in plaintext.

Once you do this, go ahead and download your credentials from the Google Drive API page. Please name this file "credentials.json" to match the name in the Python file(alternatively you could choose your own name, but then you would have to change the name in the Python file as well).

###### Adding the Device to the Python Script

Currently, at the bottom of the python script, there is a line that checks if the worksheet title is 100 or 200(these numbers refer to the IDs of the devices). If another device is added, please go ahead and add the ID of this device to this condition. For example, if a device with an ID of 300 is added, you would change the line to the following:

if(worksheet.title == "100" or worksheet.title == "200" or worksheet.title == "300"):

The sleep function at the bottom is set to 265 because on average, it takes 35 seconds to collect data, and since a new data point is added every 5 minutes, the entire data set will be read in just as the new data point is added. Feel free to experiment a little with this time if you would like, but be aware that significant changes to it will cause the Geovisualization system to not function properly and could cause the program to crash.

###### app.js file

There is a conditional in the io.sockets.on function. This checks that the data is in order and  the first measurement has device ID 100 and the last measurement has the ID of the most recently added device. You can change the "200" to the ID of the most recently added device.

###### index.html changes

The web map will contain the bulk of the changes. First, you will want to add 2 spin boxes for each new device. Each box will need a label, and you can follow the example in the index.html file for this.

You will also want to add a new chart for each measurement for the new device. You can follow the examples where the charts are generated as a guide for creating the new chart. 

Right at the beginning of the definition of the function called refresh in the script, there are initializations for a variety of lists, and they have names such as Date_arr, Humidity, Date_arr2, Humidity2, etc. You will want to follow these examples to create arrays for new mesurements(e.g. Date_arr3, Humidity3).

There is a for loop that iterates over the length of the data, and checks if the IDtag is 100 or 200. You can add another conditional that checks for the IDtag of the new device, and push it to the new arrays that you created in the above step(e.g. for device with IDtag 300, you can push to Date_arr3 and Humidity3).

There are jquery functions directly below the for loop. You can add the IDs of the new spin boxes and set the min and max values to the ends of the data, following the example for the other spin boxes. There is also an onchange function below these jquery functions, and the purpose of these is to refresh the webpage and chart the data in the selected range instead of waiting 5 minutes for the next automatic refresh. You can add the new spin boxes to this so that they update the charts as well. 

There are 4 variables, start_time1, end_time1, start_time2, and end_time2. These values collect the start and end dates that the user has entered into the spin boxes. You will need to add another start time and end time for each new device.

You will see a comment that says "checks validity of dates". The purpose of this is to make sure that the user does not pick a date that is outside of the range of data points. The code for this ends with double backslash (//) characters. For each new spin box, you will need to do this error checking. You can copy-paste all of the code for device one and change all instances of start_index1 and end_index1 to the names that you nave picked for the new start and end indices, and you can change Date_arr to the name that you have picked for the new date array (e.g. Date_arr3). Finally, you can change the spinner names from #spinner1_start and #spinner1_end to the names of the new spinners that you have created.

For each device, there are arrays called temp_Humidity, temp_TempC, etc. These are created because when billboard creates the charts, it deletes the data from the arrays, so it needs to be stored in a backup array so that it can later be used for the map.

There are conditionals to check if the start index and end index are valid in the date arrays. If they are not, then they are rounded to the nearest value in the array so that the chart has data that it can parse. When the arrays are being passed to the charts, they are sliced so that only values that the user chose will be charted.

For each measurement, there are 2 charts, one for each device. If you would like to add another device, you can reuse the code for generating either of the first two charts, but you would use the date array specific to the new device along with the array that contains the weather data from that device.

To generate the map, you would create another select list and another marker color for each new device added.  You would assign these to the the measurement chosen by the user when they click the button(this has already been set up, you would just need to assign the new select list to the measurement specific to the new device).

You would then assign the marker color based on the most recent measurement obtained from the new device. This has already been done for the first two devices, so you can reuse this and change the select list, select index and marker color to match that of the new device. You would also need to make sure that the date array that you use for the range is specific to the new device.

The final thing that you would need to do is add the new marker to the map. you can reuse the code for the creation of either marker or marker2, but you need to change the color and fillcolor to be specific to the new device. Also, the popup would need to be specific to the device, so for example, if the device that you are adding is the third device, you would make the popup say "Device 3 " + selection + ": " + select_list3[select_index3].

#####Adding a New Measurement

The following steps would need to be taken if you would like to put a new measurement on the web map:

First of all, you would need to look up the exact name of the measurement in the spreadsheet.

###### index.html

First, you would need to add another button above the web map. This can be done with the following code:

<button type="button" id="'insert ID here'">Measurement Name</button>

You would then need to add one chart per device for the new measurement. You can follow the examples of the existing charts for how to do this.

In the javascript, you would need to add the event listener for the new measurement, so when the button is clicked, you would call select_var on the new measurement so that the map is refreshed to show the new data.

You would then create a new array per device for the new measurement. Be sure to initialize it with a string that describes the measurement.

In the for loop that iterates through the data, you would push the data to these arrays. In order to get the measurements from the data, you would use the name of the measurement from the spreadsheet as a reference. So if you wanted to add albedo, you can follow the examples, but instead of 'temp', for example, you would write 'albedo'.   

You would then create a temp array per device for the same reason mentioned in the section for adding new devices, which is that billboard consumes the data from the arrays and deletes it. You would also add the measurement to the section where the arrays are sliced to only contain the dates that the user wants.

Then you can generate one chart per device with billboard for the new measurement. The existing charts can be replicated and only the bindto and columns fields would need to be changed to match the names of  the new arrays.

On the map, you would add another case statement for the new measurement, and in it, you would assign the select lists to the temporary arrays that you created before generating the billboard charts. When assigning the legend buckets, set them up so that each bucket has roughly the same amount of measurements(you may need to spend some time visually examining the data to do this).

Finally, you would need to update the legend. You can add another case statement for the new measurement, and you can reuse the code from any previous case statement, and all you would have to change is the word after case and the first line after the case statement, which is div.innerHTML += ''<b>Your new measurement</b>.

##### Testing changes locally

If you want to test out that your changes have worked, you can go to the directory where you are running the code in the terminal window and type node app.js. Then, wait 30 seconds for the webpage to gather data, and then go to the following URL: localhost:5040. 

#### Future Improvements

As mentioned above, the most likely improvements would be to add more devices and to add more measurements collected from the devices. However, there are a number of other features that could be added to the map in the future.

##### Slider Bar

Currently, the user can pick the date range that they want based on the spin boxes above the web map. While this is straightforward to use, if the user wants to jump over a large range of data at once, a slider bar may come in handy instead of them having to repeatedly click an arrow or manually type in a date. For this, I would recommend noUiSlider, as it provides an easy to use interface that allows the user to pick dates without worrying if they are out of bounds.

##### MongoDB

Currently, the data is pipelined from a Google Spreadsheet into a csv file and then through node.js onto the web page. This works, but Google Spreadsheet comes with data limits, so as the data becomes extremely large, it could begin to lose integrity. MongoDB has much higher data limits than Google Spreadsheet, so an additional step for the data could be to send it from the Google Spreadsheet into MongoDB, then from there directly into node.js and finally onto the webpage. 




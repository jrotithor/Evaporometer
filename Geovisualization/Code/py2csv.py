#Author: Jaydeep Rotithor
#Description: A python script that is used in order to extract data from the Google Spreadsheet into a csv file


import time
import gspread
from oauth2client.service_account import ServiceAccountCredentials
import csv

scope = ['https://spreadsheets.google.com/feeds']

#Authentication so that the spreadsheet can be used

credentials = ServiceAccountCredentials.from_json_keyfile_name('credentials.json', scope)

gc = gspread.authorize(credentials)

#Creates a loop that pulls the data from the spreadsheet into the csv file once every 5 minutes

while True:

	dataSheet = gc.open_by_url('https://docs.google.com/spreadsheets/d/1kkD8Tql4uJ7usnIM5TLM35XSYinU2_JmILoUw8eEjg4/edit#gid=922068760')

	worksheetList1 = dataSheet.worksheets()

	file = 'sample_data.csv'

	f = open(file, 'w')


	for worksheet in worksheetList1:
		if(worksheet.title == "100" or worksheet.title == "200"):
			writer = csv.writer(f)
			writer.writerows(worksheet.get_all_values())
			print(worksheet.title)
			
		
	time.sleep(265);

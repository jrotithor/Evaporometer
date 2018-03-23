import urllib.request
import time
import gspread
from oauth2client.service_account import ServiceAccountCredentials
import csv

scope = ['https://spreadsheets.google.com/feeds']

credentials = ServiceAccountCredentials.from_json_keyfile_name('credentials.json', scope)

gc = gspread.authorize(credentials)

while True:

	dataSheet = gc.open_by_url('https://docs.google.com/spreadsheets/d/15B3YorfMgMyoejXc_zqHkT6cVJuLfV4HsMUm8evOZow/edit#gid=0')

	worksheetList1 = dataSheet.worksheets()

	file = 'sample_data.csv'

	f = open(file, 'w')


	for worksheet in worksheetList1:
		writer = csv.writer(f)
		writer.writerows(worksheet.get_all_values())
		
	print("got here")	
	time.sleep(3);

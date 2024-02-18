function doGet(e) {
  Logger.log(JSON.stringify(e));
  var result = 'Ok';

  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  } else {
    var sheet_id = 'Enter Your Sheet ID Here';
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();

    var rowData = [];
    var currDate = new Date();

    rowData[0] = currDate; // Date in column A

    var currTime = Utilities.formatDate(currDate, "Enter Your Location Data Here", 'HH:mm:ss');
    rowData[1] = currTime; // Time in column B

    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);

      switch (param) {
        case 'temp1':
          rowData[2] = value; // Temperature for Sensor 1 in column C
          result = 'Temperature for Sensor 1 Written on column C';
          break;
        case 'temp2':
          rowData[3] = value; // Temperature for Sensor 2 in column D
          result = 'Temperature for Sensor 2 Written on column D';
          break;
        case 'temp3':
          rowData[4] = value; // Temperature for Sensor 3 in column E
          result = 'Temperature for Sensor 3 Written on column E';
          break;
        case 'temp4':
          rowData[5] = value; // Temperature for Sensor 4 in column F
          result = 'Temperature for Sensor 4 Written on column F';
          break;
        case 'temp5':
          rowData[6] = value; // Temperature for Sensor 5 in column G
          result = 'Temperature for Sensor 5 Written on column G';
          break;
        case 'temp6':
          rowData[7] = value; // Temperature for Sensor 6 in column H
          result = 'Temperature for Sensor 6 Written on column H';
          break;
        case 'voltage':
          rowData[8] = value; // Voltage in column I
          result += 'Voltage Written on column I';
          break;

        default:
          result = "unsupported parameter";
      }
    }

    Logger.log(JSON.stringify(rowData));
    sheet.insertRowBefore(2);
    var newRange = sheet.getRange(2, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }

  return ContentService.createTextOutput(result);
}

function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, "");
}

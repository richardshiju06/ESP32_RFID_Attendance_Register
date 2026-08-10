//Code to add in Appscript for google sheets

function doGet(e) { 
  Logger.log(JSON.stringify(e));
  var result = 'OK';
  if (e.parameter == 'undefined') {
    result = 'No_Parameters';
  }
  else {
    var sheet_id = ''YOUR_GOOGLE_SHEET_ID'; 	// Spreadsheet ID.
    var sheet_UD = 'User_Data';  // Sheet name for user data.
    var sheet_AT = 'Attendance';  // Sheet name for attendance.

    var sheet_open = SpreadsheetApp.openById(sheet_id);
    var sheet_user_data = sheet_open.getSheetByName(sheet_UD);
    var sheet_attendence = sheet_open.getSheetByName(sheet_AT);
    
    // Time zone for date/time formatting
    var TIME_ZONE = "Asia/Kolkata";
    // Ensure the spreadsheet uses the same timezone (requires permission)
    try {
      sheet_open.setSpreadsheetTimeZone(TIME_ZONE);
    } catch (err) {
      // If the script cannot change the sheet timezone, we'll still format times explicitly.
      Logger.log('Could not set spreadsheet timezone: ' + err);
    }
    
    // sts_val is a variable to hold the status sent by ESP32.
    // sts_val will contain "reg" or "atc".
    // "reg" = new user registration.
    // "atc" = attendance (time in and time out).
    var sts_val = "";
    
    // uid_val is a variable to hold the UID of the RFID card or keychain sent by the ESP32.
    var uid_val = "";
    
    // UID storage column.
    var uid_column = "B";
    
    // Variable to retrieve the "Time In" value from the sheet.
    var TI_val = "";
    // Variable to retrieve the "Date" value from the sheet.
    var Date_val = "";
    
    //----------------------------------------Retrieves the value of the parameter sent by the ESP32.
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'sts':
          sts_val = value;
          break;

        case 'uid':
          uid_val = value;
          break;

        default:
          // result += ",unsupported_parameter";
      }
    }
    //----------------------------------------
    
    //----------------------------------------Conditions for registering new users.
    if (sts_val == 'reg') {
      var check_new_UID = checkUID(sheet_id, sheet_UD, 2, uid_val);
      
      // Conditions when the UID has been registered. Then registration was cancelled.
      if (check_new_UID == true) {
        result += ",regErr01"; // Err_01 = UID is already registered.
        
        // Sends response payload to ESP32.
        return ContentService.createTextOutput(result);
      }

      // Writes the new user's UID to the "user data" sheet.
      var getLastRowUIDCol = findLastRow(sheet_id, sheet_UD, uid_column);  // Look for a row to write the new user's UID.
      var newUID = sheet_open.getRange(uid_column + (getLastRowUIDCol + 1));
      newUID.setValue(uid_val);
      result += ",R_Successful";
      
      // Sends response payload to ESP32.
      return ContentService.createTextOutput(result);
    }
    //----------------------------------------

    //----------------------------------------Conditions for filling attendance (Time In and Time Out).
    if (sts_val == 'atc') {
      // Checks whether the UID is already registered in the "user data" sheet.
      // findUID(Spreadsheet ID, sheet name, index column, UID value)
      // index column : 1 = column A, 2 = column B and so on.
      var FUID = findUID(sheet_id, sheet_UD, 2, uid_val);
      
      // "(FUID == -1)" means that the UID has not been registered in the "user data" sheet, so attendance filling is rejected.
      if (FUID == -1) {
        result += ",atcErr01"; // atcErr01 = UID not registered.
        return ContentService.createTextOutput(result);
      } else {
        // After the UID has been checked and the result is that the UID has been registered,
        // then take the "name" of the UID owner from the "user data" sheet.
        // The name of the UID owner is in column "A" on the "user data" sheet.
        var get_Range = sheet_user_data.getRange("A" + (FUID+2));
        var user_name_by_UID = get_Range.getValue();

        // Variables to determine attendance filling, whether to fill in "Time In", "Time Out" or attendance has been completed for today.
        var enter_data = "time_in";
        
        // Variable to get row position. This is used to fill in "Time Out".
        var num_row;
        
        // Current instant (Date object) and formatted strings in IST for responses and comparisons
        var now = new Date();
        var Curr_Date = Utilities.formatDate(now, TIME_ZONE, 'dd/MM/yyyy');
        var Curr_Time = Utilities.formatDate(now, TIME_ZONE, 'HH:mm:ss');

        // Variable to get all the data from the "attendance" sheet.
        var data = sheet_attendence.getDataRange().getDisplayValues();
        
        //..................Check whether "Time In" or "Time Out" is filled in.
        if (data.length > 1) {
          for (var i = 0; i < data.length; i++) {
            if (data[i][1] == uid_val) {
              if (data[i][2] == Curr_Date) {
                if (data[i][4] == "") {
                  Date_val = data[i][2];
                  TI_val = data[i][3];
                  enter_data = "time_out";
                  num_row = i + 1;
                  break;
                } else {
                  enter_data = "finish";
                }
              }
            }
          }
        }
        //..................
        
        //..................Conditions for filling in "Time In" attendance.
        if (enter_data == "time_in") {
          sheet_attendence.insertRows(2);

          // Write actual Date objects: spreadsheet timezone is set above, and we'll format cells to show date/time.
          sheet_attendence.getRange("A2").setValue(user_name_by_UID);
          sheet_attendence.getRange("B2").setValue(uid_val);

          // Put the Date object in date and time columns, then set number formats
          sheet_attendence.getRange("C2").setValue(now);
          sheet_attendence.getRange("D2").setValue(now);

          // Set display formats so C shows date and D shows time
          sheet_attendence.getRange("C2").setNumberFormat("dd/MM/yyyy");
          sheet_attendence.getRange("D2").setNumberFormat("HH:mm:ss");

          SpreadsheetApp.flush();
          
          // Sends response payload to ESP32 (use the formatted strings)
          result += ",TI_Successful" + "," + user_name_by_UID + "," + Curr_Date + "," + Curr_Time;
          return ContentService.createTextOutput(result);
        }
        //..................
        
        //..................Conditions for filling in "Time Out" attendance.
        if (enter_data == "time_out") {
          // Write actual Date object to Time Out column and set format
          sheet_attendence.getRange("E" + num_row).setValue(now);
          sheet_attendence.getRange("E" + num_row).setNumberFormat("HH:mm:ss");

          result += ",TO_Successful" + "," + user_name_by_UID + "," + Date_val + "," + TI_val + "," + Curr_Time;
          
          // Sends response payload to ESP32.
          return ContentService.createTextOutput(result);
        }
        //..................
        
        //..................Condition when "Time In" and "Time Out" are filled.
        if (enter_data == "finish") {
          result += ",atcInf01"; // atcInf01 = You have completed your attendance record for today (Time In and Time Out have been filled in for today).
          
          // Sends response payload to ESP32.
          return ContentService.createTextOutput(result);
        }
        //..................
      }
    }
    //----------------------------------------
  }
}
//________________________________________________________________________________

//________________________________________________________________________________stripQuotes()
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}
//________________________________________________________________________________

//________________________________________________________________________________findLastRow()
// Function to find the last row in a certain column.
// Reference : https://www.jsowl.com/find-the-last-row-of-a-single-column-in-google-sheets-in-apps-script/
function findLastRow(id_sheet, name_sheet, name_column) {
  var spreadsheet = SpreadsheetApp.openById(id_sheet);
  var sheet = spreadsheet.getSheetByName(name_sheet);
  var lastRow = sheet.getLastRow();

  var range = sheet.getRange(name_column + lastRow);

  if (range.getValue() !== "") {
    return lastRow;
  } else {
    return range.getNextDataCell(SpreadsheetApp.Direction.UP).getRow();
  }
}
//________________________________________________________________________________

//________________________________________________________________________________findUID() 
// Reference : https://stackoverflow.com/a/29546373
function findUID(id_sheet, name_sheet, column_index, searchString) {
  var open_sheet = SpreadsheetApp.openById(id_sheet);
  var sheet = open_sheet.getSheetByName(name_sheet);
  var columnValues = sheet.getRange(2, column_index, sheet.getLastRow()).getValues();  // 1st is header row.
  var searchResult = columnValues.findIndex(searchString);  // Row Index - 2.

  return searchResult;
}
//________________________________________________________________________________

//________________________________________________________________________________checkUID()
// Reference : https://stackoverflow.com/a/29546373
function checkUID(id_sheet, name_sheet, column_index, searchString) {
  var open_sheet = SpreadsheetApp.openById(id_sheet);
  var sheet = open_sheet.getSheetByName(name_sheet); 
  var columnValues = sheet.getRange(2, column_index, sheet.getLastRow()).getValues();  // 1st is header row.
  var searchResult = columnValues.findIndex(searchString);  // Row Index - 2.

  if(searchResult != -1) {
    // searchResult + 2 is row index.
    sheet.setActiveRange(sheet.getRange(searchResult + 2, 3)).setValue("UID has been registered in this row.");
    return true;
  } else {
    return false;
  }
}
//________________________________________________________________________________

//________________________________________________________________________________findIndex()
Array.prototype.findIndex = function(search){
  if(search == "") return false;
  for (var i=0; i<this.length; i++)
    if (this[i].toString().indexOf(search) > -1 ) return i;

  return -1;
}
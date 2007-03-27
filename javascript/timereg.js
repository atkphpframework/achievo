function setPeriod()
{
  input = document.getElementById("defaultperiod").value;
  alert(input);

  if (substr(input,0,3) == "tri")
  {
    // we deal with a trimester selection
    trimesternr = substr(input,3,1);
    alert(trimesternr);
    switch (trimesternr)
    {
      case "1": alert("test"); break;
    }
  }

  //date_setValue("startdate", start["y"], start["m"], start["d"]);
  //date_setValue("enddate", start["y"], start["m"], start["d"]);
}

function setStartEndDates(id, name, currentdate)
{
  /* current date attribute inputs */
  input = Array();
  input["d"] = document.getElementById(id+"[day]").value;
  input["m"] = document.getElementById(id+"[month]").value;
  input["y"] = document.getElementById(id+"[year]").value;

  switch (eval('document.entryform.'+name+'.value'))
  {
    case "0":
      break;
    case "1":
      if (id == "enddate")
      {
        start = calculateDate(input["y"], input["m"], input["d"], 7, 0, 0);
        date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
      }
      else
      {
        start = calculateDate(input["y"], input["m"], input["d"], 7, 0, 1);
        date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
      }
      break;
    case "2":
      if (id == "enddate")
      {
        start = calculateDate(input["y"], input["m"], input["d"], 14, 0, 0);
        date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
      }
      else
      {
        start = calculateDate(input["y"], input["m"], input["d"], 14, 0, 1);
        date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
      }
      break;
    case "3":
      if (id == "enddate")
      {
        start = calculateDate(input["y"], input["m"], input["d"], 21, 0, 0);
        date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
      }
      else
      {
        start = calculateDate(input["y"], input["m"], input["d"], 21, 0, 1);
        date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
      }
      break;
    case "4":
      if (id == "enddate")
      {
        start = calculateDate(input["y"], input["m"], input["d"], 28, 0, 0);
        date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
      }
      else
      {
        start = calculateDate(input["y"], input["m"], input["d"], 28, 0, 1);
        date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
      }
      break;
    case "5":
      if (id == "enddate")
      {
        start = calculateDate(input["y"], input["m"], input["d"], 0, 1, 0);
        date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
      }
      else
      {
        start = calculateDate(input["y"], input["m"], input["d"], 0, 1, 1);
        date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
      }
      break;
    case "6":
      if (id == "enddate")
      {
        start = calculateDate(input["y"], input["m"], input["d"], 0, 2, 0);
        date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
      }
      else
      {
        start = calculateDate(input["y"], input["m"], input["d"], 0, 2, 1);
        date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
      }
      break;
    case "7":
    if (id == "enddate")
    {
      start = calculateDate(input["y"], input["m"], input["d"], 0, 3, 0);
      date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
    }
    else
    {
      start = calculateDate(input["y"], input["m"], input["d"], 0, 3, 1);
      date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
    }
    break;
    case "8":
    if (id == "enddate")
    {
      start = calculateDate(input["y"], input["m"], input["d"], 0, 6, 0);
      date_setValue("startdate", start["y"], start["m"], start["d"], currentdate);
    }
    else
    {
      start = calculateDate(input["y"], input["m"], input["d"], 0, 6, 1);
      date_setValue("enddate", start["y"], start["m"], start["d"], currentdate);
    }
    break;
  }
}

/*
 * This function calculates a new date ndays or nmonths prior to or after the input date
 */
function calculateDate(year, month, day, ndays, nmonths, after)
{
  var newdate = new Date(year, month-1, day);
  if (ndays != 0)
  {
    if (after == 1) newdate.setDate(newdate.getDate()+ndays);
    else newdate.setDate(newdate.getDate()-ndays);
  }
  else
  {
    if (after == 1) newdate.setMonth(newdate.getMonth()+nmonths);
    else newdate.setMonth(newdate.getMonth()-nmonths);
  }

  output = Array();
  output["d"] = newdate.getDate();
  output["m"] = newdate.getMonth()+1;
  output["y"] = newdate.getFullYear();
  return output;
}

function date_setValue(id, year, month, day, currentdate)
{
  month = parseInt(month, 10); // remove leading '0'
  day = parseInt(day, 10);

  format = eval('atkdateattribute_'+id+'.format');

  var dayel = document.getElementById(id+"[day]");
  var monthel = document.getElementById(id+"[month]");
  var yearel = document.getElementById(id+"[year]");

  // set year
  yearel.value = year;

  // I need to call this to calculate the correct months and days
  // after setting the year.
  // TODO FIXME: I don't have the arr ('enddate') and format parameters here!!
  AdjustDate(yearel, id, format, 0, 0, false);

  // set month
  for (i=0; i<monthel.options.length;  i++)
  {
    if (monthel.options[i].value==month)
    {
      monthel.selectedIndex = i;
    }
  }

  // I need to call this to calculate the correct days
  // after setting the month.
  // TODO FIXME: !!
  AdjustDate(monthel, id, format, 0, currentdate, false);

  // set day
  for (i=0; i<dayel.options.length;  i++)
  {
    if (dayel.options[i].value==day)
    {
      dayel.selectedIndex = i;
    }
  }
}
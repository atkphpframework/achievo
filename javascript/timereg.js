if (!window.Achievo) {
    var Achievo = new Object();
}

Achievo.Timereg = {
    /**
     * Sets the start or enddate using the other one plus or minus a period
     */
    setStartEndDates: function(startdate, enddate, period, direction) {
        // Determine the source and target attributes
        source = (direction == -1) ? enddate : startdate;
        target = (direction == -1) ? startdate : enddate;

        // Read the date from the source attribute
        sourcedate = Array();
        sourcedate["day"] = document.getElementById(source + "[day]").value;
        sourcedate["month"] = document.getElementById(source + "[month]").value;
        sourcedate["year"] = document.getElementById(source + "[year]").value;

        // Apply the period mutation to determine the targetdate
        switch (period)
        {
            case "0":
                break;
            case "1":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 7 * direction, 0);
                break;
            case "2":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 14 * direction, 0);
                break;
            case "3":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 21 * direction, 0);
                break;
            case "4":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 28 * direction, 0);
                break;
            case "5":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 0, 1 * direction);
                break;
            case "6":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 0, 2 * direction);
                break;
            case "7":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 0, 3 * direction);
                break;
            case "8":
                targetdate = Achievo.Timereg.calculateDate(sourcedate, 0, 6 * direction);
                break;
        }

        // Set the targetdate into the target attribute
        ATK.DateAttribute.setValue(target, targetdate);
    },
    /**
     * This function calculates a new date deltadays or deltamonths prior to or after the input date
     */
    calculateDate: function(input, deltadays, deltamonths) {
        // Convert the input array to a date object
        var dateobj = new Date(input["year"], input["month"] - 1, input["day"]);

        // Apply the mutations to the date object
        if (deltadays != 0)
            dateobj.setDate(dateobj.getDate() + deltadays);
        if (deltamonths != 0)
            dateobj.setMonth(dateobj.getMonth() + deltamonths);

        // Convert the date object back to an array
        output = Array();
        output["day"] = dateobj.getDate();
        output["month"] = dateobj.getMonth() + 1;
        output["year"] = dateobj.getFullYear();
        return output;
    }
}
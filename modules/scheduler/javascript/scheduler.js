function get_object(name)
{
	if (document.getElementById)
	{
		return document.getElementById(name);
 	}
 	else if (document.all)
	{
  		return document.all[name];
 	}
 	else if (document.layers)
	{
  		return document.layers[name];
	}
	return false;
}

function getRecur()
{
  if(get_object('recur_once').checked==true)
  {
    return 'once';
  }
  else if(get_object('recur_daily').checked==true)
  {
    return 'daily';
  }
  else if(get_object('recur_weekly').checked==true)
  {
    return 'weekly';
  }
  else if(get_object('recur_monthly_date').checked==true)
  {
    return 'monthly_date';
  }
  else if(get_object('recur_monthly_day').checked==true)
  {
    return 'monthly_day';
  }
  else if(get_object('recur_yearly_date').checked==true)
  {
    return 'yearly_date';
  }
  else if(get_object('recur_yearly_day').checked==true)
  {
    return 'yearly_day';
  }
  else
  {
    return 'once';
  }
}


function change_recur(recur)
{
  if(recur==null)
  {
    recur=getRecur();
  }

  switch(recur)
	{
		case 'once':
		  // For once, we need:
		  // - startdate, enddate
		  // - starttime, endtime, allday
      //disable_cyclus_startdate(true);
      disable_cyclus_enddate(true);
      disable_every(true);
      disable_month_time(true);
      disable_weekday(true);
      disable_startday(true);
      disable_endday(true);
      disable_startmonth(true);
      disable_endmonth(true);
			get_object('every_day').style.display='none';
      get_object('every_week').style.display='none';
			get_object('every_month').style.display='none';
			get_object('every_year').style.display='none';
      break;
    case 'daily':
      // For daily, we need:
      // - starttime, endtime, allday
      // - cyclus_startdate, cyclus_enddate
      // - every
      //disable_cyclus_startdate(false);
      disable_cyclus_enddate(false);
      disable_every(false);
      disable_month_time(true);
      disable_weekday(true);
      disable_startday(true);
      disable_endday(true);
      disable_startmonth(true);
      disable_endmonth(true);
			get_object('every_day').style.display='inline';
      get_object('every_week').style.display='none';
			get_object('every_month').style.display='none';
			get_object('every_year').style.display='none';
      break;

    case 'weekly':
      // For weekly, we need:
      // - starttime, endtime, allday
      // - cyclus_startdate, cyclus_enddate
      // - every
      // - weekday
      //disable_cyclus_startdate(false);
      disable_cyclus_enddate(false);
      disable_every(false);
      disable_month_time(true);
      disable_weekday(false);
      disable_startday(true);
      disable_endday(true);
      disable_startmonth(true);
      disable_endmonth(true);
			get_object('every_day').style.display='none';
      get_object('every_week').style.display='inline';
			get_object('every_month').style.display='none';
			get_object('every_year').style.display='none';
      break;
    case 'monthly_date':
      // For montly_date, we need:
      // - startday, endday
      // - starttime, endtime, allday
      // - cyclus_startdate, cyclus_enddate
      // - every
      //disable_cyclus_startdate(false);
      disable_cyclus_enddate(false);
      disable_every(false);
      disable_month_time(true);
      disable_weekday(true);
      disable_startday(false);
      disable_endday(false);
      disable_startmonth(true);
      disable_endmonth(true);
			get_object('every_day').style.display='none';
      get_object('every_week').style.display='none';
			get_object('every_month').style.display='inline';
			get_object('every_year').style.display='none';
      break;
    case 'monthly_day':
      // For montly_date, we need:
      // - starttime, endtime, allday
      // - cyclus_startdate, cyclus_enddate
      // - every
      // - month_time,weekday
      //disable_cyclus_startdate(false);
      disable_cyclus_enddate(false);
      disable_every(false);
      disable_month_time(false);
      disable_weekday(false);
      disable_startday(true);
      disable_endday(true);
      disable_startmonth(true);
      disable_endmonth(true);
			get_object('every_day').style.display='none';
      get_object('every_week').style.display='none';
			get_object('every_month').style.display='inline';
			get_object('every_year').style.display='none';
			break;
    case 'yearly_date':
      // For yearly_date, we need:
      // - startday, endday
      // - startmonth, endmonth
      // - starttime, endtime, allday
      // - cyclus_startdate, cyclus_enddate
      // - every
      //disable_cyclus_startdate(false);
      disable_cyclus_enddate(false);
      disable_every(true);
      disable_month_time(true);
      disable_weekday(true);
      disable_startday(false);
      disable_endday(false);
      disable_startmonth(false);
      disable_endmonth(false);
			get_object('every_day').style.display='none';
      get_object('every_week').style.display='none';
			get_object('every_month').style.display='none';
			get_object('every_year').style.display='none';
      break;
    case 'yearly_day':
      // For yearly_day, we need:
      // - starttime, endtime, allday
      // - cyclus_startdate, cyclus_enddate
      // - every
      // - weekday, month
      //disable_cyclus_startdate(false);
      disable_cyclus_enddate(false);
      disable_every(false);
      disable_month_time(false);
      disable_weekday(false);
      disable_startday(true);
      disable_endday(true);
      disable_startmonth(false);
      disable_endmonth(true);
			get_object('every_day').style.display='none';
      get_object('every_week').style.display='none';
			get_object('every_month').style.display='none';
			get_object('every_year').style.display='inline';
      break;
  }
  showTab(getCurrentTab());
}

function disable_cyclus_startdate(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_cyclus_startdate').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_cyclus_startdate').className='section_cyclus';
  }
}

function disable_cyclus_enddate(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_cyclus_enddate').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_cyclus_enddate').className='section_cyclus';
  }
}

function disable_startday(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_startday').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_startday').className='section_cyclus';
  }
}

function disable_endday(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_endday').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_endday').className='section_cyclus';
  }
}

function disable_startmonth(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_startmonth').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_startmonth').className='section_cyclus';
  }
}

function disable_endmonth(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_endmonth').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_endmonth').className='section_cyclus';
  }
}

function disable_every(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_every').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_every').className='section_cyclus';
  }
}

function disable_month_time(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_month_time').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_month_time').className='section_cyclus';
  }
}

function disable_weekday(disable)
{
  if(disable)
  {
    get_object('ar_cyclus_AE_weekday').className='';
  }
  else
  {
    get_object('ar_cyclus_AE_weekday').className='section_cyclus';
  }
}

function change_time()
{
  if(get_object('allday').checked==true)
  {
    get_object('ar_starttime').className='';
    get_object('ar_endtime').className='';
  }
  else
  {
    get_object('ar_starttime').className='default';
    get_object('ar_endtime').className='default';
  }
  showTab(getCurrentTab());
}

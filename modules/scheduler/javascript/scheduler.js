
function getRecur()
{
  recur = $F('recur');
  if(recur==null) return 'once';
  return recur;
}

function show_once()
{
  $('ar_cyclus_AE_daily_choice').className='';
  $('ar_cyclus_AE_weekly_choice').className='';
  $('ar_cyclus_AE_monthly_choice').className='';
  $('ar_cyclus_AE_yearly_choice').className='';
  $('ar_cyclus_AE_end_choice').className='';
}

function show_daily()
{
  $('ar_cyclus_AE_daily_choice').className='section_cyclus';
  $('ar_cyclus_AE_weekly_choice').className='';
  $('ar_cyclus_AE_monthly_choice').className='';
  $('ar_cyclus_AE_yearly_choice').className='';
  $('ar_cyclus_AE_end_choice').className='section_cyclus';
}

function show_weekly()
{
  $('ar_cyclus_AE_daily_choice').className='';
  $('ar_cyclus_AE_weekly_choice').className='section_cyclus';
  $('ar_cyclus_AE_monthly_choice').className='';
  $('ar_cyclus_AE_yearly_choice').className='';
  $('ar_cyclus_AE_end_choice').className='section_cyclus';
}

function show_monthly()
{
  $('ar_cyclus_AE_daily_choice').className='';
  $('ar_cyclus_AE_weekly_choice').className='';
  $('ar_cyclus_AE_monthly_choice').className='section_cyclus';
  $('ar_cyclus_AE_yearly_choice').className='';
  $('ar_cyclus_AE_end_choice').className='section_cyclus';
}

function show_yearly()
{
  $('ar_cyclus_AE_daily_choice').className='';
  $('ar_cyclus_AE_weekly_choice').className='';
  $('ar_cyclus_AE_monthly_choice').className='';
  $('ar_cyclus_AE_yearly_choice').className='section_cyclus';
  $('ar_cyclus_AE_end_choice').className='section_cyclus';
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
      show_once();
      break;
    case 'daily':
      show_daily();
      break;
    case 'weekly':
      show_weekly();
      break;
    case 'monthly':
      show_monthly();
      break;
    case 'yearly':
      show_yearly();
      break;
  }
  showTab(getCurrentTab());
}

function getDuration()
{
  duration = $F('duration');
  if(duration==null) return -1;
  return duration;
}

function change_duration(value)
{
  if(value==null)
  {
    value=getDuration();
  }
  if(value == -1)
  {
    $('enddate').show();
    $('endtime').show();
  }
  else
  {
    $('enddate').hide();
    $('endtime').hide();
  }
  showTab(getCurrentTab());
}


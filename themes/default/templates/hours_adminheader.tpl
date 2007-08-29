<form name="dayview" method="post" action="{atkdispatchfile}">
  {$sessionform}
  <table border=0 cellpadding=0 cellspacing=0 width="100%" class="table" id="timereg-adminheader">
    <tr>
      <td align="left" valign="top">
        <a href="{$yesterdayurl}">{atktext previousday}</a>
        {if $todayurl && $tomorrowurl}
          &nbsp;&nbsp;<a href="{$todayurl}">{atktext id=today node=houradmin}</a>
          &nbsp;&nbsp;<a href="{$tomorrowurl}">{atktext nextday}</a>
        {/if}
        &nbsp;&nbsp;<a href="{$weekviewurl}">{atktext gotoweekview}</a>
      </td>
      <td align="right" valign="top">
        &nbsp;&nbsp;{$userselect}
        &nbsp;&nbsp;{$datejumper}
        &nbsp;&nbsp;<input type="submit" value="{atktext "goto"}">
      </td>
    </tr>
  </table>
</form>
<b>{$currentdate}</b>{$lockicon}
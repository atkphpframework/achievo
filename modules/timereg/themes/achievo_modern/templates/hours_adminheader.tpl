<form name="dayview" method="post" action="dispatch.php" style="position: relative;">
{$sessionform}
<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">
  <tr>
    <td valign="top" align="left">
      <a href="{$yesterdayurl}">{atktext previousday}</a>&nbsp;
      {if $todayurl && $tomorrowurl}
        <a href="{$todayurl}">{atktext id=today node=houradmin}</a>&nbsp;
        <a href="{$tomorrowurl}">{atktext nextday}</a>
      {/if}
      <a href="{$weekviewurl}">{atktext gotoweekview}</a>
    </td>
    <td valign="top" align="right">
      {$userselect}&nbsp;{$datejumper}&nbsp;<input type="submit" value="{atktext "goto"}">
    </td>
</table>
<div class="currentDate">{$currentdate} {$lockicon}</div>
</form>
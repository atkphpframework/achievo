<form name="dayview" method="post" action="dispatch.php">
<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table" id="timereg-adminheader">
  <tr>
    <td align="left" valign="top">
      <a href="{$yesterdayurl}">{atktext yesterday}</a>
      {if $todayurl && $tomorrowurl}
      <a href="{$todayurl}">{atktext id=today node=houradmin}</a>
      <a href="{$tomorrowurl}">{atktext nextday}</a>
      {/if}
      <a href="{$weekviewurl}">{atktext gotoweekview}
      </td>
      <td align="right" valign="top">
      {$session_form}
      {$userselect}&nbsp;{$datejumper}&nbsp;<input type="submit" value="{atktext "goto"}">
      </td>
    </tr>
  </table>
</form>
<b>{$currentdate}</b>{$lockicon}
<form name="dayview" method="post" action="dispatch.php" style="position: relative;">

<div id="changeView">
  <a href="{$yesterdayurl}">{atktext yesterday}</a>&nbsp;
  {if $todayurl && $tomorrowurl}
    <a href="{$todayurl}">{atktext id=today node=houradmin}</a>&nbsp;
    <a href="{$tomorrowurl}">{atktext nextday}</a>
  {/if}
    <a href="{$weekviewurl}">{atktext gotoweekview}</a>
</div>

<div class="currentDate">{$currentdate} {$lockicon}</div>
{$session_form}{$userselect}&nbsp;{$datejumper}&nbsp;<input type="submit" value="{atktext "goto"}">

</form>
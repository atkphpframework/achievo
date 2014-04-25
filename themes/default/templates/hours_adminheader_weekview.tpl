<form name="weekview" method="post" action="{atkdispatchfile}">
    {$session_form}
    <table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">
        <tr>
            <td valign="top" align="left">
                <a href="{$prevweekurl}">{atktext previousweek}</a>
                &nbsp;&nbsp;<a href="{$thisweekurl}">{atktext thisweek}</a>
                {if $nextweekurl}&nbsp;&nbsp;<a href="{$nextweekurl}">{atktext nextweek}</a>{/if}
                &nbsp;&nbsp;<a href="{$dayviewurl}">{atktext dayview}</a>

                {if $lockurl}         &nbsp;&nbsp;<a href="{$lockurl}"><b>{atktext lock}</b></a>
                {elseif $unlockurl}  &nbsp;&nbsp;<a href="{$unlockurl}"><b>{atktext unlock}</b></a>
                        {/if}

            </td>
            <td valign="top" align="right">
                &nbsp;&nbsp;{$userselect}
                &nbsp;&nbsp;{$datejumper}
                &nbsp;&nbsp;<input type="submit" value="{atktext goto}">
            </td>
        </tr>
    </table>
</form>
<br/>
<b>{$curdate}</b>
{$lockicon} {if $locktext}({$locktext}){/if}
<br><br>

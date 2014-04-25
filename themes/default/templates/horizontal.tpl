{$formstart}
<TABLE border="0" cellpadding="0" cellspacing="0"><TR valign="{$valign}">
        {foreach name=verticalblocks from=$blocks item=block}
            <TD{if !$smarty.foreach.verticalblocks.first} style="padding-left: 20px"{/if}>{$block}</TD>
            {/foreach}
    </TR></TABLE>
{$formend}
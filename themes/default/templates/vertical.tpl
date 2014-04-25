{$formstart}
{foreach from=$blocks item=block}
    <TABLE border="0" cellpadding="0" cellspacing="0"><TR valign="top"><TD align="{$align}">
                {$block}
                </TD></TR></TABLE>
        <br>
        {/foreach}
            {$formend}
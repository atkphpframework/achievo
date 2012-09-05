<b>{atktext id="last_viewed" module="crm"}:</b>&nbsp;
{foreach  name=lastvieweditems from=$items item=item}
<a class="lastviewedlink" href="{$item.url}" accessKey="{$smarty.foreach.lastvieweditems.iteration}" title="[Alt+{$smarty.foreach.lastvieweditems.iteration}]">{$item.summary|truncate:15:'':true}</a>
{if $smarty.foreach.lastvieweditems.last eq false}
&nbsp;|&nbsp;
{/if}

{/foreach}
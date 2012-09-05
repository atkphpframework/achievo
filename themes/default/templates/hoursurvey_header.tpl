{if $saved_criteria.load_criteria}
<div class="load_criteria">
{$saved_criteria.label_load_criteria}: 
{$saved_criteria.load_criteria}
{if $saved_criteria.forget_criteria}
  <a href="{$saved_criteria.forget_criteria}" title="{$saved_criteria.label_forget_criteria}"><img class="recordlist" border="0" title="Verwijder" alt="Verwijder" src="{atkthemeicon name='delete' type='recordlist'}" /></a>
{/if}
</div>
{/if}
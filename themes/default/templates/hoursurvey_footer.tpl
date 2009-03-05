{if $saved_criteria.toggle_save_criteria }
<div class="save_criteria">
{$saved_criteria.toggle_save_criteria} {$saved_criteria.label_save_criteria} {$saved_criteria.save_criteria}
</div>
<div>
{$saved_criteria.dynamic_dates} {$saved_criteria.label_dynamic_dates}  
</div>
{if $saved_criteria.all_users}
<div>
{$saved_criteria.all_users} {$saved_criteria.label_all_users}  
</div>
{/if}
{/if}
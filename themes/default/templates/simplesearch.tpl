<form action="dispatch.php"{if $targetframe!=""}target="{$targetframe}"{/if}>
<input type="hidden" name="atknodetype" value="search.search">
<input type="hidden" name="atkaction" value="search">
{$session_form}
<input name="searchstring" type="text" size="18" value="{$searchstring}">&nbsp;<input type="submit" value="{atktext search}">
</form>
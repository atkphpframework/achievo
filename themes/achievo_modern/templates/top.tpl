<!-- <table id="top" width="100%" cellspacing="0" cellpadding="0" border="1">
  <tr>
    <td id="top-left" width="190">
      <img src="themes/achievo_modern/images/top-left.png" />
    </td>
    <td id="top-right" background="themes/achievo_modern/images/top-right.png" style="background-repeat: repeat-x;">
<table width="100%" height="100%">
<tr>
<td align="center" valign="middle" height="50%"> 
  <span id="top-center">{$centerpiece}</span> | 
  <span id="top-logout"><a href="index.php?atklogout=1" target="{$logouttarget}">{atktext logout}</a></span> | {atktext search}
  <span id="top-search">{$searchpiece}</span>
</td>
</tr>
<tr>
<td valign="bottom" align="right" height="50%">
<br />
<br />
<br />
<span id="top-user" style="padding-right: 20px;">{atktext logged_in_as} {$user}</span>
</td>
</tr>
</table>
</td>
</tr>
</table>

-->
<div id="banner">
  <img src="{$themedir}images/logo.jpg" alt="Logo Achievo" />
  <div id="topLinks">
    <span id="top-center">{if $centerpiece_links.setup}{$centerpiece_links.setup}{else}{$centerpiece_links.pim} &nbsp;|&nbsp; {$centerpiece_links.userprefs}{/if}</span>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;  
    <span id="top-logout"><a href="index.php?atklogout=1" target="{$logouttarget}">{atktext logout}</a></span>&nbsp;&nbsp;|&nbsp;&nbsp;{atktext search}&nbsp;
    <span id="top-search">{$searchpiece}</span>
  </div>
  <div id="loginBox">
    {atktext logged_in_as}: {$user}
  </div>
</div>

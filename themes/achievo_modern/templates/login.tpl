{literal}
<style>
body
{
{/literal}
	padding: 24px;
	background: #fff url({$themedir}images/bodyPattern.gif) repeat left top;
{literal}
}
</style>
{/literal}

<div id='loginform' style="background: #EBEBEB url({$themedir}images/logoGrijs.png) no-repeat 40px 20px;">
<form action="{$formurl}" method="post">
  <div id="loginform-title">{atktext login_form}</div>
  <div id="loginform-content">
  {if $auth_max_loginattempts_exceeded}
    {$auth_max_loginattempts_exceeded}
  {else}
    <table cellpadding="0" cellspacing="0" border="0"><tr>
    <td class="loginformLabel">{atktext username}:</td><td class="loginformField">{$userfield}</td>
    </tr><tr>
    <td class="loginformLabel">{atktext password}:</td><td class="loginformField"><input class="loginform" type="password" size="15" name="auth_pw" value=""></td>
    </tr><tr>
    <td class="loginformLabel"></td><td><input class="button" type="submit" value="{atktext login}"></td>
    </tr></table>
  {$atksessionformvars}
  {/if}
  </div>
</div>

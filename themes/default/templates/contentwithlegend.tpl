<TABLE border="0" cellpadding="0" cellspacing="0"><TR valign="top"><TD>
  {$content}
</TD><TD style="padding-left: 20px;">
  <DIV style="border: 1px #000000 solid; background: #FFFFFF"><TABLE border="0" cellPadding="2" cellSpacing="0"><TBODY>
  {foreach from=$legenditems key=color item=description}
  <TR>
  <TD><TABLE border="0" cellpadding="0" cellspacing="0"><tr><td bgcolor="{$color}"><IMG src="images/trans.gif" border="1" width="10" height="10"></td></tr></table></TD>
  <TD align="left"><FONT color="#000000" face="verdana" size="-1">{$description}</FONT></TD>
  </TR>
  {/foreach}
  </TBODY></TABLE></DIV>
</TD></TR></TABLE>
<?php

/**
 * Smarty resource plugin for fetching email templates from
 * the database. $tpl_name is parsed as a uri type of string where
 * the path to the template field is encoded as:
 *
 * code/field
 *
 * results in:
 *    SELECT field FROM email_template WHERE code='code' ...
 * -------------------------------------------------------------
 */
function smarty_resource_templatedb_fetch($tpl_name, &$tpl_source, &$smarty, $default=false)
{
   $_url = parse_url($tpl_name);

   // (required) expected syntax: table/source_field
   $_path_items = explode('/', $_url['path']);
   $code = $_path_items[0];
   $field = $_path_items[1];

   if (!isset($smarty->templatedb_driver)) {
      $smarty->templatedb_driver = &atkGetDb();
   }

   $tmp_source = $smarty->templatedb_driver->getrows("SELECT $field FROM email_template WHERE code='$code' LIMIT 1;");
   if (count($tmp_source)==1)
   {
      $tmp_source = array_values($tmp_source[0]);
      $tpl_source = $tmp_source[0];
      return true;
   } else {
      return $default;
   }
}

function smarty_resource_templatedb_source($tpl_name, &$tpl_source, &$smarty)
{
   if (smarty_resource_templatedb_fetch($tpl_name, $tpl_source, $smarty)) {
      $tpl_source = stripslashes($tpl_source);
      return true;
   }
   return false;
}

function smarty_resource_templatedb_timestamp($tpl_name, &$tpl_source, &$smarty)
{
   $_url = parse_url($tpl_name);
   // (required) expected syntax: code/field
   $_path_items = explode('/', $_url['path']);
   $code = $_path_items[0];
   $field = 'UNIX_TIMESTAMP(lastupdatedon)';
   $tpl_name = "$code/$field";
   return smarty_resource_templatedb_fetch($tpl_name, $tpl_source, $smarty, microtime());
}

function smarty_resource_templatedb_secure($tpl_name, &$smarty)
{
   return true;
}
function smarty_resource_templatedb_trusted($tpl_name, &$smarty)
{
   return true;
}

?>
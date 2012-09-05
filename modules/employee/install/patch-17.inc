<?php
// Update the new language field of patch-16 with the default language from the config

$sql = "UPDATE person SET lng='".atkconfig("language")."' WHERE role='employee' AND (lng IS NULL OR lng='')";
$db->query($sql);

// Update theme when it's empty or one of the depricated themes

$sql = "UPDATE person SET theme='".atkconfig("defaulttheme")."'
        WHERE role='employee'
          AND  (theme IN ('slash','blurb','ananda','blackbox','kuro5hin','newsforge','papered','rainyd')
                OR theme IS NULL
                OR theme='')";
$db->query($sql);

?>
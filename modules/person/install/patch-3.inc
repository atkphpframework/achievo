<?php

  $setup->installNode("person.title");
  $setup->addColumn("person", "title_id", "int(10)", false, 0);
  
  $titles = $db->getrows("SELECT DISTINCT title FROM person WHERE title IS NOT NULL AND title<>''");
  
  foreach ($titles as $title)
  {
    atkdebug("Inserting $title");
    $id = $db->nextid("title");
    $db->query("INSERT INTO title (id, title) VALUES ($id, '".$db->escapeSQL($title["title"])."')");
    $db->query("UPDATE person SET title_id = $id WHERE title='".$db->escapeSQL($title["title"])."'");
  }
  
  $setup->dropColumn("person", "title");

?>
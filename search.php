<?
  include_once("atk.inc");  
  atksession();  
  atksecure();
  include_once("./theme.inc");
 
  $g_layout->output('<html>');
  $g_layout->head($txt_app_title);
  $g_layout->body();
  
  $g_layout->ui_top(text("search"));
  
  $searchresults = "You're looking for '$searchstring'. But this is not yet implemented!";
  
  // Hmm.. how are we going to do this..
  
  // I prefer something like:
  //
  // for all modules including the main module
  // {
  //   for all nodes in the module
  //   {
  //     $obj = getNode($node[$i]);
  //     $recs = $obj->searchDb($searchstring);
  //     $searchresults.= '<br><br>'.$obj->recordListThatAlreadyContainsAllApplicableActions($recs);
  //   }
  // }
  //
  // Or in other words, make searching a native atkNode feature.
  
  $g_layout->output($searchresults);
  
  $g_layout->ui_bottom();
  $g_layout->output('</body>');
  $g_layout->output('</html>');
  
  $g_layout->outputFlush();

?>
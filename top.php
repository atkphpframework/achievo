<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * This file is the skeleton top frame file, which you can copy
   * to your application dir and modify if necessary. By default,
   * it displays the currently logged-in user and a logout link.
   *
   * @package atk
   * @subpackage skel
   *
   * @author Ivo Jansch <ivo@achievo.org>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal includes.
   */
  $config_atkroot = "./";
  include_once("atk.inc"); 
  include("version.inc");

  atksession();
  atksecure();   
  
  $page = &atknew("atk.ui.atkpage");  
  $ui = &atknew("atk.ui.atkui");  
  $theme = &atkTheme::getInstance();
  $output = &atkOutput::getInstance();
  
  $page->register_style($theme->stylePath("style.css"));
  $page->register_stylecode("form{display: inline;}");
  $page->register_style($theme->stylePath("top.css"));
  
  //Backwards compatible $content, that is what will render when the box.tpl is used instead of a top.tpl
  $loggedin = atktext("logged_in_as").": <b>".$g_user["name"]."</b>";  
  $content = '<br>'.$loggedin.' &nbsp; <a href="index.php?atklogout=1" target="_top">'.ucfirst(text("logout", "", "atk")).' </a>&nbsp;';

  if ($g_user["name"]!="administrator")
  {
    $centerpiece.= href(dispatch_url("pim.pim", "pim"), text("pim","","core"), SESSION_NEW, false, 'target="main"').'&nbsp; &nbsp; &nbsp;';
    $centerpiece.= href(dispatch_url("employee.userprefs", "edit", array("atkselector" => "person.id='".$g_user["id"]."'")), text('userprefs','','core'), SESSION_NEW, false, 'target="main"');
  }
  else
  {
    // Administrator has a link to setup.php
    $centerpiece.= href("setup.php", text("setup","","core"), SESSION_NEW, false, 'target="_top"');
  }
  $content.=$centerpiece;
  
  $searchnode = getNode("search.search");
  $searchpiece = $searchnode->simpleSearchForm("", "main", SESSION_NEW);
  $content.="&nbsp;&nbsp;&nbsp; ".$searchpiece;
  
  $title = text("app_title","","core")." ".$achievo_version;
  ($achievo_state!=="stable")?$title.=" ($achievo_state)":"";
  
  $top = $ui->renderTop(array("content"=> $content,
  							  "logintext" => atktext("logged_in_as"),
                              "logouttext" => ucfirst(atktext("logout")),
                              "logoutlink" => "index.php?atklogout=1",
                              "logouttarget"=>"_top",
                              "centerpiece"=>$centerpiece,
                              "searchpiece"=>$searchpiece,
                              "title" => $title,
  							  "user"   => $g_user["name"]));
 
  $page->addContent($top);

  $output->output($page->render(atktext('app_title'), true));
  
  $output->outputFlush();
?>
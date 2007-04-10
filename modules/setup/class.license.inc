<?php

class license extends atkNode
{
  function license() 
  {
    $this->atkNode("license",NF_NO_SECURITY);
  }
  
  // Page functions
  function action_view(&$handler)
  {
    $ui = &$this->getUi();
    if (is_object($ui))
    {
      $theme = &atkTheme::getInstance();
      $page = &$this->getPage();
      $page->register_style($theme->stylePath("style.css"));
      $page->head(atktext("cicence","setup"));
      $page->body();

     
      $box = $ui->renderBox(array("title"=>$this->text($this->m_type)." - ".$this->text($this->m_view),
                                             "content"=>$this->getLicenseText()));
      $actionpage = $this->renderActionPage("view", array($box));
      $page->addContent($actionpage);
    }
    else
    {
       atkerror("ui object failure");
    }
  }
  
  function getLicenseText()
  {
    $license = file(atkconfig("atkroot")."doc/LICENSE");
    $content="";
    for ($i=0;$i<count($license);$i++)
    {
      $content.='<br>'.str_replace("", "", $license[$i]);
    }
    return $content;    
  }
  
}

?>
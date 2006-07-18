<?php
class xmlfunctions
{
  function xmlfunctions()
  {
  }
  
  function exportXML($holidays)
  {
    if(count($holidays)==0) return "";
    return "<xml>
              <test>dit is een test</test>
            </xml>";
    
  }
  
  function importXML()
  {
  } 
}


?>
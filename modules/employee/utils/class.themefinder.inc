<?

class themefinder
{
  var $m_themes = array();

  function visitFile($file)
  {
    $exploded = explode('/',$file);
    $filename = array_pop($exploded);
    $filepath = implode('/',$exploded).'/';

    if (strtolower($filename)==='themedef.inc')
    {
      $themename = array_pop($exploded);
      if (!in_array($themename,$this->m_themes)) $this->m_themes[] = $themename;
    }
  }

  function getThemes()
  {
    sort($this->m_themes);
    return $this->m_themes;
  }
}

?>
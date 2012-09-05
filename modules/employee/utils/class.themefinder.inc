<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage employee
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Class for finding available themes
 * @package achievo
 * @subpackage employee
 */
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
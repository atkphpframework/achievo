<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage person
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

atkimport("atk.attributes.atkattribute");

/**
 * User id attribute
 * When a userid is saved it will replace the space by an underscore
 *
 * @package achievo
 * @subpackage person
 */
class useridAttribute extends atkAttribute
{
  /**
   * Constructor
   *
   * @param string $name Attribute name
   * @param int $flags Flags
   */
  function useridAttribute($name, $flags=0)
  {
    $this->atkAttribute($name, $flags, 15);
  }

  /**
   * Value 2 database
   *
   * @param array $rec Record
   * @return string User id without spaces
   */
  function value2db($rec)
  {
    return escapeSQL(str_replace(" ", "_", $rec["userid"]));
  }
}


?>

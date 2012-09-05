<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage todo
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

  /**
   * Render todo's of the current user for the PIM screen
   *
   * @return string
   */
  function pimTodos()
  {
    $todonode = atkGetNode("todo.todo");
    return $todonode->renderPim();
  }

?>
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

  require_once("achievotools.inc");
  require_once(moduleDir("todo")."class.todo.inc");
  useattrib("atkdummyattribute");
  useattrib("atkupdatestampattribute");
  userelation("atkmanytoonerelation");

  /**
   * Todo history, to keep track of changes to todo's.
   * Rewriten to be an extension of todo class by Ivo.
   *
   * @author Dan Frame <dan_frame@jabil.com>
   * @author Ivo Jansch <ivo@achievo.org>
   * @package achievo
   * @subpackage todo
   */
  class todo_history extends todo
  {
    /**
     * Constructor
     *
     * @return void
     */
    function todo_history()
    {
      $this->todo("todo_history"); // call baseclass constructor, but with new name.
      $this->m_type = "todo_history";
      $this->addFlag(NF_READONLY);
      $this->add(new atkManyToOneRelation("todoid", "todo.todo",AF_READONLY|AF_OBLIGATORY|AF_SEARCHABLE));

      // We also need to change the updated attribute, because now we no longer want it hide_list,
      // and we don't want to store the current date, but the original date that the todo was
      // last changed before it ended up in the history.
      $this->remove("updated");
      $this->add(new atkUpdateStampAttribute ("updated"));

      $this->setTable("todo_history");

      // Ordering by id makes sure we sort in order of history.
      $this->setOrder("id");
      $this->addSecurityMap("view",'todo.admin');

    }
    
    /**
     * Override title_display for not showing a link
     * @return string
     * @param array $record Record
     * @param string $mode Mode
     */
    function title_display($record,$mode)
    {
      return $record["title"];
    }


    /**
     * do nothing. we need to define this dummy, otherwise todo::postUpdate
     * might be called, which creates another history item.
     *
     * @param array $rec Record
     * @return boolean
     */
    function postUpdate($rec)
    {
      return true;
    }

    /**
     * do nothing. we need to define this dummy, otherwise todo::postAdd
     * might be called, which creates another history item.
     *
     * @param array $rec Record
     * @return boolean
     */
    function postAdd($rec)
    {
      return true;
    }

  }
?>